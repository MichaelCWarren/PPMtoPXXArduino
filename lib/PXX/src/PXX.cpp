#include <Arduino.h>
#include <PXX.h>

#define PPM_CENTER 1500
#define PPM_LOW 817
#define PPM_HIGH 2182
#define PPM_HIGH_ADJUSTED (PPM_HIGH - PPM_LOW)
#define PXX_CHANNEL_WIDTH 2048
#define PXX_UPPER_LOW 2049
#define PXX_UPPER_HIGH 4094
#define PXX_LOWER_LOW 1
#define PXX_LOWER_HIGH 2046

const uint16_t CRC_Short[] =
    {
        0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
        0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7};

uint16_t CRCTable(uint8_t val)
{
    return CRC_Short[val & 0x0F] ^ (0x1081 * (val >> 4));
}

void PXX_Class::crc(uint8_t data)
{
    pcmCrc = (pcmCrc << 8) ^ CRCTable((pcmCrc >> 8) ^ data);
}

void USART_Init(long baud)
{
    int _baud = (16000000 / (2 * baud)) - 1;

    UBRR0 = 0;

    /* Setting the XCKn port pin as output, enables master mode. */
    // XCKn_DDR |= (1<<XCKn);
    // DDRD = DDRD | B00000010;

    /* Set MSPI mode of operation and SPI data mode 0. */
    UCSR0C = (1 << UMSEL01) | (1 << UMSEL00) | (1 << UDORD0);

    /* Enable receiver and transmitter. */
    UCSR0B = (1 << TXEN0);
    /* Set baud rate. */
    /* IMPORTANT: The Baud Rate must be set after the transmitter is enabled */
    UBRR0 = _baud;
}

void USART_Send(uint8_t data)
{
    /* Put data into buffer, sends the data */
    UDR0 = data;

    // Wait for the buffer to be empty
    while (!(UCSR0A & (1 << UDRE0)))
        ;
}

void PXX_Class::begin()
{
    USART_Init(125000);
}

void PXX_Class::putPcmSerialBit(uint8_t bit)
{
    serialByte >>= 1;
    if (bit & 1)
    {
        serialByte |= 0x80;
    }

    if (++serialBitCount >= 8)
    {
        pulses[length++] = serialByte;
        serialBitCount = 0;
    }
}

// 8uS/bit 01 = 0, 001 = 1
void PXX_Class::putPcmPart(uint8_t value)
{
    putPcmSerialBit(0);

    if (value)
    {
        putPcmSerialBit(0);
    }

    putPcmSerialBit(1);
}

void PXX_Class::putPcmFlush()
{
    while (serialBitCount != 0)
    {
        putPcmSerialBit(1);
    }
}

void PXX_Class::putPcmBit(uint8_t bit)
{
    if (bit)
    {
        pcmOnesCount += 1;
        putPcmPart(1);
    }
    else
    {
        pcmOnesCount = 0;
        putPcmPart(0);
    }

    if (pcmOnesCount >= 5)
    {
        putPcmBit(0); // Stuff a 0 bit in
    }
}

void PXX_Class::putPcmByte(uint8_t byte)
{
    crc(byte);

    for (uint8_t i = 0; i < 8; i++)
    {
        putPcmBit(byte & 0x80);
        byte <<= 1;
    }
}

void PXX_Class::putPcmHead()
{
    // send 7E, do not CRC
    // 01111110
    putPcmPart(0);
    putPcmPart(1);
    putPcmPart(1);
    putPcmPart(1);
    putPcmPart(1);
    putPcmPart(1);
    putPcmPart(1);
    putPcmPart(0);
}

void PXX_Class::prepareChannels(int16_t channels[16], bool sendUpperChannels)
{
    uint16_t chan = 0, chan_low = 0;

    length = 0;
    pcmCrc = 0;
    pcmOnesCount = 0;

    /* Preamble */
    putPcmPart(0);
    putPcmPart(0);
    putPcmPart(0);
    putPcmPart(0);

    /* Sync */
    putPcmHead();

    // Rx Number
    putPcmByte(16);

    // FLAG1 - Fail Safe Mode, nothing currently set, maybe want to do this
    putPcmByte(0);

    // FLAG2
    putPcmByte(0);

    // PPM
    for (int i = 0; i < 8; i++)
    {

        int channelPPM = channels[(sendUpperChannels ? (8 + i) : i)];
        float convertedChan = ((float(channelPPM) - float(PPM_LOW)) / (float(PPM_HIGH_ADJUSTED))) * float(PXX_CHANNEL_WIDTH);
        chan = limit(1,
                     convertedChan,
                     PXX_CHANNEL_WIDTH - 1) +
               (sendUpperChannels ? 2048 : 0);

        if (i & 1)
        {
            putPcmByte(chan_low);                               // Low byte of channel
            putPcmByte(((chan_low >> 8) & 0x0F) | (chan << 4)); // 4 bits each from 2 channels
            putPcmByte(chan >> 4);                              // High byte of channel
        }
        else
        {
            chan_low = chan;
        }
    }

    uint8_t extraFlags = 0;
    extraFlags |= (1 << 2); // Channels 9-16 Enabled
    putPcmByte(extraFlags);

    // CRC16
    chan = pcmCrc;
    putPcmByte(chan >> 8);
    putPcmByte(chan);

    // Sync
    putPcmHead();
    putPcmFlush();
}

void PXX_Class::send(int16_t channels[16])
{
    prepareChannels(channels, false);
    sendPulses();
    prepareChannels(channels, true);
    sendPulses();
}

void PXX_Class::sendPulses()
{
    for (int i = 0; i < length; i++)
    {
        USART_Send(pulses[i]);
    }
}

uint16_t PXX_Class::limit(uint16_t low, uint16_t val, uint16_t high)
{
    if (val < low)
    {
        return low;
    }

    if (val > high)
    {
        return high;
    }

    return val;
}

PXX_Class PXX;
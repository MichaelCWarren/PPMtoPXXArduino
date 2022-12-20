#ifndef PXX_h
#define PXX_h

#include <Arduino.h>

#define PXX_FAILSAFE_HOLD 0
#define PXX_FAILSAFE_NO_PULSE 1
#define PXX_FAILSAFE_MID_RANGE 2

class PXX_Class
{
public:
    void begin();
    void send(int16_t channels[16], bool bind, bool failsafe);

private:
    void sendPulses();
    void prepareChannels(int16_t channels[16], bool sendUpperChannels, bool bind, bool failsafe);
    uint8_t pulses[64];
    int length = 0;
    uint16_t pcmCrc;
    uint32_t pcmOnesCount;
    uint16_t serialByte;
    uint16_t serialBitCount;

    void crc(uint8_t data);
    void putPcmSerialBit(uint8_t bit);
    void putPcmPart(uint8_t value);
    void putPcmFlush();
    void putPcmBit(uint8_t bit);
    void putPcmByte(uint8_t byte);
    void putPcmHead();
    uint16_t limit(uint16_t low, uint16_t val, uint16_t high);
    void preparePulses(int16_t channels[8]);
};

extern PXX_Class PXX;

#endif

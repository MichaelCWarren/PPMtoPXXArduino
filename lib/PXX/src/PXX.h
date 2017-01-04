#include "Arduino.h"
#include "SoftwareSerial.h"

class PXX_Class
{
    public:
        PXX_Class();
        void begin();
        void send(int16_t channels[16], int channelCount);

    private:
        uint8_t  pulses[64];
        int length = 0;
        uint16_t pcmCrc;
        uint32_t pcmOnesCount;
        uint16_t serialByte;
        uint16_t serialBitCount;
        SoftwareSerial serial;

        void crc(uint8_t data);
        void putPcmSerialBit(uint8_t bit);
        void putPcmPart(uint8_t value);
        void putPcmFlush();
        void putPcmBit(uint8_t bit);
        void putPcmByte(uint8_t byte);
        void putPcmHead();
        uint16_t limit(uint16_t low,uint16_t val, uint16_t high);
        void preparePulses(int16_t channels[16], int channelCount);
};

extern PXX_Class PXX;

#include "Arduino.h"
#include <CPPM.h>
#include <PXX.h>

void setup()
{
    Serial.begin(115200);

    PXX.begin();
    CPPM.begin();
}


void loop()
{
    CPPM.cycle();
    if (CPPM.synchronized())
    {
        int16_t channels[16] = {0,0,0,0,0,0,0,1000,1000,1000,1000,1000,1000,1000,1000,1000};
        for(int i = 0; i < 7; i++)
        {
            channels[i] = CPPM.read_us(i);
        }

        PXX.send(channels, 16);
    }

    delay(9);
}

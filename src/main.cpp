#include <Arduino.h>
#include <CPPM.h>
#include <PXX.h>

int16_t channels[16] = {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};

void setup()
{
    CPPM.begin();
    PXX.begin();
}

void loop()
{
    CPPM.cycle();
    if (CPPM.synchronized())
    {
        for(int i = 0; i < 7; i++)
        {
            channels[i] = CPPM.read_us(i);
        }

        PXX.send(channels);
    }

    delay(8);
}

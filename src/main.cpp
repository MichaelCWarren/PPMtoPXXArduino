#include <Arduino.h>
#include <CPPM.h>
#include <PXX.h>

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
        int16_t channels[16] = {2000,2000,2000,2000,2000,2000,2000,1000,1000,1000,1000,1000,1000,1000,1000,1000};
        for(int i = 1; i < 7; i++)
        {
            channels[i] = CPPM.read_us(i);
        }

        PXX.send(channels);
    }
    delay(9);
}

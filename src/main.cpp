#include <Arduino.h>
#include <CPPM.h>
#include <PXX.h>

int16_t channels[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool doCPP = true;

void setup()
{
    CPPM.begin();
    PXX.begin();
}

void loop()
{
    if (doCPP)
    {
        CPPM.cycle();
        if (CPPM.synchronized())
        {
            for (int i = 0; i < 7; i++)
            {
                channels[i] = CPPM.read_us(i);
            }

            channels[8] = ((analogRead(0) > 200 ? 1 : 0) * 1000) + 1000;
            channels[9] = ((analogRead(1) > 200 ? 1 : 0) * 1000) + 1000;
            channels[10] = ((analogRead(2) > 200 ? 1 : 0) * 1000) + 1000;
            channels[11] = ((analogRead(3) > 200 ? 1 : 0) * 1000) + 1000;
        }
    }
    else
    {
        PXX.send(channels);
    }

    doCPP = !doCPP;

    delay(2);
}

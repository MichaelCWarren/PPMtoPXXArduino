#include <Arduino.h>
#include <CPPM.h>
#include <PXX.h>

#define TRANSMITTER_CHANNELS 7
#define INITIAL_CHANNEL_VALUE 1500
#define BIND_PIN 2

int16_t channels[16] = {INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE, 
                        INITIAL_CHANNEL_VALUE};

int16_t failsafe[16] = {PXX_FAILSAFE_HOLD, //A
                        PXX_FAILSAFE_HOLD, //E
                        PXX_FAILSAFE_HOLD, //T
                        PXX_FAILSAFE_HOLD, //R
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_HOLD,
                        PXX_FAILSAFE_NO_PULSE}; //Last channel will go low when failsafe is active

void checkBind();
bool bind = false;


void setup()
{
    CPPM.begin(TRANSMITTER_CHANNELS);
    PXX.begin();

    pinMode(BIND_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BIND_PIN), checkBind, CHANGE); 
}

void loop()
{
    static bool checkCPPM = true;
    static uint16_t sendCounter = 0;
    if (checkCPPM)
    {
        if (CPPM.ok())
        {
            CPPM.read(channels);

            channels[8] = ((analogRead(0) > 200 ? 1 : 0) * 1000) + 1000;
            channels[9] = ((analogRead(1) > 200 ? 1 : 0) * 1000) + 1000;
            channels[10] = ((analogRead(2) > 200 ? 1 : 0) * 1000) + 1000;
            channels[11] = ((analogRead(3) > 200 ? 1 : 0) * 1000) + 1000;
        }
    }
    else
    {
        if (sendCounter % 1000 == 0)
        {
            PXX.send(failsafe, bind, true);
        }
        else
        {
            PXX.send(channels, bind, false);
            sendCounter++;
        }
    }

    checkCPPM = !checkCPPM;

    delay(2);
}

void checkBind() 
{
    bind = digitalRead(BIND_PIN);
}

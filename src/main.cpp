#include <Arduino.h>
#include <CPPM.h>
#include <PXX.h>

int16_t channels[16] = {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};

void initTimer2()
{
    noInterrupts();
    //Reset timer settings
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;

    //9ms Timer Compare Value
    OCR2A = 140;
    //CTC mode timer
    TCCR2B |= (1 << WGM22);
    // set up timer with prescaler = 1024
    TCCR2B |= (1 << CS22)|(1 << CS20);
    //Enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
    interrupts();
}

ISR(TIMER2_COMPA_vect)
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
}

void setup()
{
    initTimer2();
    CPPM.begin();
    PXX.begin();
}

void loop() { }

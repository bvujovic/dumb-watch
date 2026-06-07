#include "Buzzer.h"

Buzzer::Buzzer(byte pinBuzzer)
{
    _pinBuzzer = pinBuzzer;
    pinMode(pinBuzzer, OUTPUT);
    digitalWrite(pinBuzzer, LOW);
}

void Buzzer::buzz(ulong ms)
{
    digitalWrite(_pinBuzzer, HIGH);
    delay(ms);
    digitalWrite(_pinBuzzer, LOW);
}

void Buzzer::buzz(ulong ms, int count)
{
    for (int i = 0; i < count; ++i)
    {
        buzz(ms);
        if (i != count - 1)
            delay(ms);
    }
}

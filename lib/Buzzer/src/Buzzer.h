#pragma once

#include <Arduino.h>

#define MS_ITV_LONG 1000
#define MS_ITV_MEDIUM 333
#define MS_ITV_SHORT 100

class Buzzer
{
private:
    byte _pinBuzzer;

public:
    Buzzer(byte pinBuzzer);

    void buzz(ulong ms);
    void buzz(ulong ms, int count);
};

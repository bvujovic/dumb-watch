#pragma once

#include <Arduino.h>
#include <TM1637Display.h>

class Display
{
private:
    TM1637Display* disp;

    uint8_t encodeCharToSegments(char c);

public:
    Display(byte pinClock, byte pinDio);

    void clear();
    void clear(uint32_t itvDelay);

    void nums(int x, int y);
    void string(const char *str);
    // void displayTimerName(const TimerItem &timer);

};

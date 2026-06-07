#pragma once

#include <Arduino.h>
#include <TM1637Display.h> // smougenot/TM1637 @ 0.0.0-alpha+sha.9486982048

class Display
{
private:
    TM1637Display* disp;

    uint8_t encodeCharToSegments(char c);

public:
    /// @param brightness – A number from 0 (lowes brightness) to 7 (highest brightness) 
    Display(byte pinClock, byte pinDio, byte brightness = 1);

    void clear();
    void clear(uint32_t itvDelay);

    void nums(int x, int y);
    void string(const char *str);
    // void displayTimerName(const TimerItem &timer);

};

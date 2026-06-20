#pragma once

#include <Arduino.h>
#include <TM1637Display.h> // smougenot/TM1637 @ 0.0.0-alpha+sha.9486982048

class Display
{
private:
  TM1637Display *disp;

  uint8_t encodeCharToSegments(char c);

public:
  Display(byte pinClock, byte pinDio);

  /// @param brightness – A number from 0 (lowest brightness) to 7 (highest brightness)
  void setBrightness(byte brightness) { disp->setBrightness(brightness); }

  void clear();
  void clear(uint32_t itvDelay);

  void nums(int x, int y, bool colon = true);
  void string(const char *str, bool colon = false);
  // void setSegments(const uint8_t segments[]) { disp->setSegments(segments); }
  // void displayTimerName(const TimerItem &timer);
};

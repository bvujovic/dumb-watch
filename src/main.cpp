#include <Arduino.h>

#if defined(CONFIG_IDF_TARGET_ESP32C3)
const byte pinDispClock = 4; // TM1637 display CLK pin
const byte pinDispDio = 3;   // TM1637 display DIO pin
const byte pinDHT = 1;       // DHT sensor pin
const byte pinButton = 2;    // button pin
const byte pinBuzzer = 10;   // buzzer pin
#elif defined(CONFIG_IDF_TARGET_ESP32)
const byte pinDispClock = 32; // TM1637 display CLK pin
const byte pinDispDio = 33;   // TM1637 display DIO pin
const byte pinDHT = 25;       // DHT sensor pin
const byte pinButton = 14;    // button pin
const byte pinBuzzer = 27;    // buzzer pin
#endif

#include "Display.h"
Display display(pinDispClock, pinDispDio);

#include "Buzzer.h"
Buzzer buzzer(pinBuzzer);

class TimerItem
{
public:
  char name[4];
  int *intervals;
  int intervalCount;

  TimerItem(const char nameIn[4], int *intervalsIn, int count)
      : intervals(intervalsIn), intervalCount(count)
  {
    for (int i = 0; i < 4; ++i)
      name[i] = nameIn[i];
  }
};

#include "TempHum.h"
TempHum tempHum(pinDHT, &display);

#include "Clock.h"
Clock clck(&buzzer);

#include "OneButton.h" // lib_deps = mathertel/OneButton@^2.0.0
OneButton btn(pinButton, true);

// #define MS_ITV_LONG 1000
// #define MS_ITV_MEDIUM 333
// #define MS_ITV_SHORT 100

ulong msLastDisplayUpdate;
enum Mode
{
  TEMP_HUM,    // Display current temperature and humidity
  TIMER,       // Countdown from some given time
  CLOCK,       // Time from 00:00 or current time
  BRIGHTNESS,  // Set brightness of the screen
  CLOCK_BEEPS, // How often should clock beep: ev 5, 10, 30min
  MODE_COUNT
};
Mode mode = TEMP_HUM;

// minutes to seconds
int toSecs(double min) { return (int)(min * 60); }
// minutes to seconds: min * 60 + sec
int toSecs(int min, int sec) { return min * 60 + sec; }

TimerItem timers[] = {
    TimerItem("COFF", new int[1]{toSecs(1, 20)}, 1),
    TimerItem("EGGS", new int[1]{toSecs(3, 20)}, 1),
    // TimerItem("FREE", new int[1]{toSecs(7)}, 1), // "BLEJ"

    // rest between sets
    // TimerItem("RESS", new int[1]{60}, 1),
    // TimerItem("RESM", new int[1]{90}, 1),
    // TimerItem("RESL", new int[1]{120}, 1),

    // test how mode names look on the display
    // TimerItem("TEHU", new int[1]{60}, 1),
    // TimerItem("TIMR", new int[1]{90}, 1),
    // TimerItem("CLCK", new int[1]{120}, 1),

    // TimerItem("0012", new int[1]{12}, 1),
    // TimerItem("TEST", new int[1]{60}, 1),
};
int idxTimer = 0;
ulong msTimerStart;

void nextMode()
{
  display.clear();
  msLastDisplayUpdate = ULONG_MAX;
  mode = static_cast<Mode>((mode + 1) % MODE_COUNT);

  switch (mode)
  {
  case TIMER:
    display.string(timers[idxTimer = 0].name);
    break;

  case CLOCK:
    display.nums(0, 0);
    break;
  }
}

void clickHandler()
{
  switch (mode)
  {
  case TEMP_HUM:
    tempHum.toggleBothTempHumDisplayed();
    msLastDisplayUpdate = ULONG_MAX;
    break;

  case TIMER:
    if (++idxTimer >= sizeof(timers) / sizeof(timers[0]))
      idxTimer = 0;
    display.string(timers[idxTimer].name);
    msTimerStart = 0;
    break;

  case CLOCK:
    // if clock is currently showing time - switch to showing current time, otherwise switch to showing time since clock started
    clck.toggleClockCurrentTime();
    if (clck.getClockCurrentTime())
    {
      display.clear();
      clck.getCurrentTime();
    }
    break;

  case BRIGHTNESS:
    display.setBrightness(0);
    break;

  case CLOCK_BEEPS:
    clck.setClockBeeps(Every5min);
    break;
  }
}

void doubleClickHandler()
{
  switch (mode)
  {
  case TIMER:
    msTimerStart = millis();
    break;

  case CLOCK:
    if (!clck.getClockCurrentTime())
    {
      if (clck.isPaused())
        clck.start();
      else
        clck.pause();
      display.nums(clck.getHours(), clck.getMinutes());
    }
    break;

  case BRIGHTNESS:
    display.setBrightness(1);
    break;

  case CLOCK_BEEPS:
    clck.setClockBeeps(Every10min);
    break;
  }
}

void multiClickHandler()
{
  if (mode == BRIGHTNESS)
  {
    if (btn.getNumberClicks() == 3)
      display.setBrightness(2);
    if (btn.getNumberClicks() == 4)
      display.setBrightness(7);
    buzzer.buzz(100, btn.getNumberClicks());
  }
  if (mode == CLOCK_BEEPS)
  {
    if (btn.getNumberClicks() == 3)
      clck.setClockBeeps(Every30min);
    if (btn.getNumberClicks() == 4)
      clck.setClockBeeps(NeverBeeps);
  }
}

void setup()
{
  // Serial.begin(115200);
  display.setBrightness(1);
  tempHum.begin();
  clck.setClockBeeps(Every10min);
  clck.pause();

  btn.attachClick(clickHandler);

  btn.attachDoubleClick(doubleClickHandler);

  btn.attachLongPressStart([]()
                           { nextMode(); });

  btn.attachMultiClick(multiClickHandler);

  // set msLastDisplayUpdate to unsigned long max value, so that display will be updated immediately in the loop
  msLastDisplayUpdate = ULONG_MAX;
}

void loop()
{
  delay(20);
  btn.tick();

  switch (mode)
  {
  case TEMP_HUM:
  {
    if (msLastDisplayUpdate != ULONG_MAX && millis() - msLastDisplayUpdate < 5000)
      break;
    tempHum.display();
    msLastDisplayUpdate = millis();
    break;
  }

  case TIMER:
  {
    if (msTimerStart == 0)
      break;
    int remaining = timers[idxTimer].intervals[0] - (millis() - msTimerStart) / 1000;
    if (remaining < 0)
    {
      remaining = 0;
      msTimerStart = 0;
      buzzer.buzz(MS_ITV_MEDIUM, 2);
      display.string(timers[idxTimer].name);
    }
    else
      display.nums(remaining / 60, remaining % 60);
    break;
  }

  case CLOCK:
  {
    auto sec = millis() / 1000;
    auto displayColon = clck.isPaused() ? sec % 2 : true;
    if (clck.display())
      display.nums(clck.getHours(), clck.getMinutes(), displayColon);
  }
  break;

  case BRIGHTNESS:
    display.string("brit"); // 2nd display name option "BRGH"
    break;

  case CLOCK_BEEPS:
    display.nums(0, clck.getClockBeeps());
    break;
  }
}

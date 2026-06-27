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

#include "Timers.h"
Timers timers;

#include "TempHum.h"
TempHum tempHum(pinDHT, &display);

#include "Clock.h"
Clock clck(&buzzer);

#include "OneButton.h" // lib_deps = mathertel/OneButton@^2.0.0
OneButton btn(pinButton, true);

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

void nextMode()
{
  display.clear();
  msLastDisplayUpdate = ULONG_MAX;
  mode = static_cast<Mode>((mode + 1) % MODE_COUNT);

  switch (mode)
  {
  case TIMER:
    // display.string(timers[idxTimer = 0].name);
    timers.moveToFirst();
    display.string(timers.current().name);
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
    timers.moveToNext();
    display.string(timers.current().name);
    timers.resetTime();
    break;

  case CLOCK:
    clck.toggleClockCurrentTime();
    if (clck.getClockCurrentTime())
    {
      display.clear();
      clck.getCurrentTime();
    }
    else  
    {
      display.clear();
      delay(MS_ITV_MEDIUM);
      clck.start();
      display.nums(clck.getHours(), clck.getMinutes());
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
    // msTimerStart = millis();
    timers.startTime(millis());
    buzzer.buzz(MS_ITV_SHORT);
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
  // timers.add(TimerItem("TEST", 0, 3));
  timers.add(TimerItem("COFF", 1, 20));
  timers.add(TimerItem("EGGS", 3, 20));

  btn.attachClick(clickHandler);
  btn.attachDoubleClick(doubleClickHandler);
  btn.attachLongPressStart(nextMode);
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
    if (timers.isEmpty() || !timers.timeStarted())
      break;
    auto remaining = timers.secondsRemaining(millis());
    if (remaining < 0)
    {
      remaining = 0;
      timers.resetTime();
      buzzer.buzz(MS_ITV_MEDIUM, 2);
      display.string(timers.current().name);
    }
    else
      display.nums(remaining / 60, remaining % 60);
    break;
  }

  case CLOCK:
  {
    auto sec = millis() / 1000;
    auto displayColon = clck.isPaused() ? sec % 2 : true;
    // if (clck.display())
    //   display.nums(clck.getHours(), clck.getMinutes(), displayColon);
    if (clck.display())
    {
      display.nums(clck.getHours(), clck.getMinutes(), displayColon);
      clck.beepIN();
    }
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

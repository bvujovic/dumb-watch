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
Display display(pinDispClock, pinDispDio, 1);

#include "Buzzer.h"
Buzzer buzzer(pinBuzzer);

// void getTime()
// {
//   time(&now);             // read the current time
//   localtime_r(&now, &ti); // update the structure tm with the current time
// }

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
  TEMP_HUM, // Display current temperature and humidity
  TIMER,    // Countdown from some given time
  CLOCK,    // Time from 00:00 or current time
  MODE_COUNT
};
Mode mode = TEMP_HUM;

// minutes to seconds
int toSecs(double min) { return (int)(min * 60); }
// minutes to seconds: min * 60 + sec
int toSecs(int min, int sec) { return min * 60 + sec; }

TimerItem timers[] = {
    TimerItem("COFF", new int[1]{toSecs(1, 30)}, 1),
    TimerItem("EGGS", new int[1]{toSecs(3.5)}, 1),
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

// ulong msClockStart;
// int lastBeepedMin = -1;
// bool isClockCurrentTime = false;

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

// ulong msStartGetTime;
// const ulong msMaxGetTime = (ulong)15 * 1000;

// abort getting current time if it takes too long
// bool abortGetTimeIN()
// {
//   if (millis() > msStartGetTime + msMaxGetTime)
//   {
//     WiFi.mode(WIFI_OFF);
//     isClockCurrentTime = false;
//     msClockStart = millis();
//     buzzer.buzz(MS_ITV_SHORT, 3);
//     return true;
//   }
//   return false;
// }

void setup()
{
  // pinMode(pinBuzzer, OUTPUT);
  // digitalWrite(pinBuzzer, LOW);

  // Serial.begin(115200);
  tempHum.begin();

  btn.attachClick([]()
                  {
    switch (mode)
    {
      case TEMP_HUM:
        tempHum.toggleBothTempHumDisplayed();
        // display.clear(MS_ITV_MEDIUM);
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
        // isClockCurrentTime = !isClockCurrentTime;
        clck.toggleClockCurrentTime();
        // if (isClockCurrentTime)
        if (clck.getClockCurrentTime())
        {
          display.clear();
          clck.getCurrentTime();
        //   msStartGetTime = millis();
        //   WiFi.persistent(false);
        //   WiFi.mode(WIFI_STA);
        //   WiFi.setTxPower(WIFI_POWER_13dBm);
        //   WiFi.begin(WIFI_SSID, WIFI_PASS);
        //   while (WiFi.status() != WL_CONNECTED)
        //   {
        //     delay(1000);
        //     if (abortGetTimeIN())
        //       return;
        //   }
        //   configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
        //   setenv("TZ", MY_TZ, 1);          // Set environment variable with your time zone
        //   tzset();
        //   while (ti.tm_year < (2025 - 1900))
        //   {
        //     delay(1000);
        //     getTime();          
        //     if (abortGetTimeIN())
        //       return;
        //   }
        //   WiFi.mode(WIFI_OFF); // turn off wifi to save power, we don't need it anymore
        //   msClockStart = millis() - (ti.tm_hour * 3600 + ti.tm_min * 60 + ti.tm_sec) * 1000;
        }
        else
          clck.start();
          // msClockStart = millis();
        // lastBeepedMin = -1;
        break;
    } });

  btn.attachDoubleClick([]()
                        { 
    switch (mode)
    {
      case TIMER:
        msTimerStart = millis();
        break;
      case CLOCK:
        // msClockStart = millis();
        // lastBeepedMin = -1;
        if (clck.start())
        {
          display.clear(MS_ITV_SHORT);
          display.nums(0, 0);
          delay(MS_ITV_SHORT);
          display.clear(MS_ITV_SHORT);
        }
        break;  
    } });

  btn.attachLongPressStart([]()
                           { nextMode(); });
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
    if (clck.display())
      display.nums(clck.getHours(), clck.getMinutes());
    // if (msClockStart == 0)
    //   break;
    // int mm = (millis() - msClockStart) / 60000;
    // int hh = (mm / 60) % 24;
    // mm = mm % 60;
    // display.nums(hh, mm);
    // if (mm != lastBeepedMin && (mm % 5 == 0) && !(mm == 0 && lastBeepedMin == -1))
    // {
    //   if (mm % 30 == 0) // 00 -> 2x LONG, 30 -> 1x LONG
    //     buzzer.buzz(MS_ITV_LONG, mm == 0 ? 2 : 1);
    //   else if (mm % 10 == 0) // 10, 40 -> 1x MED, 20, 50 -> 2x MED
    //     buzzer.buzz(MS_ITV_MEDIUM, mm == 10 || mm == 40 ? 1 : 2);
    //   else // 5, 15, 25, 35, 45, 55 -> 2x SHORT
    //     buzzer.buzz(MS_ITV_SHORT, 2);
    //   lastBeepedMin = mm;
    // }
    break;
  }
  }
}

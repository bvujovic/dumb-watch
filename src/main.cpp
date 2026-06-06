#include <Arduino.h>

#if defined(CONFIG_IDF_TARGET_ESP32C3)
const byte pinClock = 4;   // TM1637 display CLK pin
const byte pinDio = 3;     // TM1637 display DIO pin
const byte pinDHT = 1;     // DHT sensor pin
const byte pinButton = 2;  // button pin
const byte pinBuzzer = 10; // buzzer pin
#elif defined(CONFIG_IDF_TARGET_ESP32)
const byte pinClock = 32;  // TM1637 display CLK pin
const byte pinDio = 33;    // TM1637 display DIO pin
const byte pinDHT = 25;    // DHT sensor pin
const byte pinButton = 14; // button pin
const byte pinBuzzer = 27; // buzzer pin
#endif

#include "Display.h"
Display display(pinClock, pinDio);

#include <WiFi.h>
// #include <CredWiFi.h>
#include <CredWiFi_HotSpot.h>
#define MY_NTP_SERVER "rs.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#include "time.h"
time_t now; // this are the seconds since Epoch (1970) - UTC
struct tm ti;

void getTime()
{
  time(&now);             // read the current time
  localtime_r(&now, &ti); // update the structure tm with the current time
}

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

#include <DHT.h>      // lib_deps = adafruit/DHT sensor library@^1.4.6
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(pinDHT, DHTTYPE);

#include "OneButton.h"          // lib_deps = mathertel/OneButton@^2.0.0
OneButton btn(pinButton, true);

#define MS_ITV_LONG 1000
#define MS_ITV_MEDIUM 333
#define MS_ITV_SHORT 100

void buzz(int ms)
{
  digitalWrite(pinBuzzer, HIGH);
  delay(ms);
  digitalWrite(pinBuzzer, LOW);
}
void buzz(int ms, int count)
{
  for (int i = 0; i < count; ++i)
  {
    buzz(ms);
    if (i != count - 1)
      delay(ms);
  }
}

ulong msLastDisplayUpdate;
enum Mode
{
  TEMP_HUM, // Display current temperature and humidity
  TIMER,    // Countdown from some given time
  CLOCK,    // Time from 00:00 or current time
  MODE_COUNT
};
Mode mode = TEMP_HUM;

bool isBothTempHumDisplayed = true; // true - display temp:hum; false - display temp w/ decimals

// minutes to seconds
int toSecs(double min) { return (int)(min * 60); }

TimerItem timers[] = {
    TimerItem("COFF", new int[1]{toSecs(2)}, 1),
    TimerItem("EGGS", new int[1]{toSecs(3.5)}, 1),
    TimerItem("FREE", new int[1]{toSecs(7)}, 1), // "BLEJ"

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

ulong msClockStart;
int lastBeepedMin = -1;
bool isClockCurrentTime = false;

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

void setup()
{
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, LOW);
  // Serial.begin(115200);
  dht.begin();

  btn.attachClick([]()
                  {
    switch (mode)
    {
      case TEMP_HUM:
        isBothTempHumDisplayed = !isBothTempHumDisplayed;
        display.clear(MS_ITV_MEDIUM);
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
        isClockCurrentTime = !isClockCurrentTime;
        if (isClockCurrentTime)
        {
          // if(ti.tm_year < (2025 - 1900)) // if time is not set yet - get time from NTP
          // {
          display.clear();
          WiFi.persistent(false);
          WiFi.mode(WIFI_STA);
          WiFi.setTxPower(WIFI_POWER_13dBm);
          WiFi.begin(WIFI_SSID, WIFI_PASS);
          while (WiFi.status() != WL_CONNECTED)
            delay(1000);
          configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
          setenv("TZ", MY_TZ, 1);          // Set environment variable with your time zone
          tzset();

          // Serial.println("Waiting for NTP time sync:");
          while (ti.tm_year < (2025 - 1900))
          {
              // Serial.print(".");
              delay(1000);
              getTime();
          }
          // Serial.println("Time synchronized!");
          // showTime();
          WiFi.mode(WIFI_OFF); // turn off wifi to save power, we don't need it anymore
          // }
          msClockStart = millis() - (ti.tm_hour * 3600 + ti.tm_min * 60 + ti.tm_sec) * 1000;
        }
        else
          msClockStart = millis();

        lastBeepedMin = -1;
        // displayNums(ti.tm_hour, ti.tm_min);
        // delay(MS_ITV_SHORT);
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
        msClockStart = millis();
        lastBeepedMin = -1;
        display.clear(MS_ITV_SHORT);
        display.nums(0, 0);
        delay(MS_ITV_SHORT);
        display.clear(MS_ITV_SHORT);
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
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    if (isnan(hum) || isnan(temp))
      temp = hum = 0;
    if (hum > 100)
      hum = 100;
    if (isBothTempHumDisplayed)
    {
      temp += 0.5; // round to nearest int
      hum += 0.5;  // round to nearest int
      display.nums(temp, hum);
    }
    else
    {
      auto intTemp = (int)temp;      // 25.46 -> 25
      auto decTemp = temp - intTemp; // 25.46 -> 0.46
      decTemp *= 100;
      display.nums(intTemp, decTemp + 0.5);
    }
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
      buzz(MS_ITV_MEDIUM, 2);
      display.string(timers[idxTimer].name);
    }
    else
      display.nums(remaining / 60, remaining % 60);
    break;
  }

  case CLOCK:
  {
    if (msClockStart == 0)
      break;
    int mm = (millis() - msClockStart) / 60000;
    int hh = (mm / 60) % 24;
    mm = mm % 60;
    display.nums(hh, mm);
    if (mm != lastBeepedMin && (mm % 5 == 0) && !(mm == 0 && lastBeepedMin == -1))
    {
      if (mm % 30 == 0) // 00 -> 2x LONG, 30 -> 1x LONG
        buzz(MS_ITV_LONG, mm == 0 ? 2 : 1);
      else if (mm % 10 == 0) // 10, 40 -> 1x MED, 20, 50 -> 2x MED
        buzz(MS_ITV_MEDIUM, mm == 10 || mm == 40 ? 1 : 2);
      else // 5, 15, 25, 35, 45, 55 -> 2x SHORT
        buzz(MS_ITV_SHORT, 2);
      lastBeepedMin = mm;
    }
    break;
  }
  }
}

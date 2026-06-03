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

#include <TM1637Display.h>
TM1637Display display(pinClock, pinDio);

#include <WiFi.h>
#include <CredWiFi.h>
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

void displayNums(int x, int y)
{
  int d = x * 100 + y;
  display.showNumberDecEx(d, 0b11100000, true);
}

uint8_t encodeCharToSegments(char c)
{
  switch (c)
  {
  case 'A':
  case 'a':
    return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
  case 'B':
  case 'b':
    return SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
  case 'C':
  case 'c':
    // return SEG_D | SEG_E | SEG_G;
    return SEG_A | SEG_D | SEG_E | SEG_F;
  case 'D':
  case 'd':
    return SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
  case 'E':
  case 'e':
    return SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
  case 'F':
  case 'f':
    return SEG_A | SEG_E | SEG_F | SEG_G;
  case 'G':
  case 'g':
    // return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F;
    return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
  case 'M':
  case 'm':
    return SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
  case 'H':
  case 'h':
    return SEG_C | SEG_E | SEG_F | SEG_G;
  case 'I':
  case 'i':
    return SEG_E;
  case 'J':
  case 'j':
    return SEG_B | SEG_C | SEG_D;
  case 'K':
  case 'k':
    // return SEG_E | SEG_F | SEG_G;
    return SEG_C | SEG_E | SEG_F | SEG_G;
  case 'L':
  case 'l':
    return SEG_D | SEG_E | SEG_F;
  case 'N':
  case 'n':
    return SEG_C | SEG_E | SEG_G;
  case 'O':
  case 'o':
    return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
  case 'P':
  case 'p':
    return SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
  case 'Q':
  case 'q':
    return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
  case 'R':
  case 'r':
    return SEG_E | SEG_G;
  case 'S':
  case 's':
    return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
  case 'T':
  case 't':
    return SEG_D | SEG_E | SEG_F | SEG_G;
  case 'U':
  case 'u':
    return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
  case 'Y':
  case 'y':
    return SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
  case 'Z':
  case 'z':
    return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
  case '0':
    return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
  case '1':
    return SEG_B | SEG_C;
  case '2':
    return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
  case '3':
    return SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
  case '4':
    return SEG_B | SEG_C | SEG_F | SEG_G;
  case '5':
    return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
  case '6':
    return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
  case '7':
    return SEG_A | SEG_B | SEG_C;
  case '8':
    return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
  case '9':
    return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
  case ' ':
    return 0;
  case '\'':
    return SEG_F;
  default:
    return 0;
  }
}

void displayTimerName(const TimerItem &timer)
{
  // if all charachers in timer.name are digits - display that data with displayNums(temp, hum)
  // maybe this should be changed: if name is e.g. "----" - display time based on interval (convert it to min:sec)
  auto allDigits = true;
  for (auto i = 0; i < 4; ++i)
    if (timer.name[i] < '0' || timer.name[i] > '9')
      allDigits = false;
  if (allDigits)
  {
    auto x = (timer.name[0] - '0') * 10 + timer.name[1] - '0';
    auto y = (timer.name[2] - '0') * 10 + timer.name[3] - '0';
    displayNums(x, y);
    return;
  }
  uint8_t segments[4];
  for (int i = 0; i < 4; ++i)
    segments[i] = encodeCharToSegments(timer.name[i]);
  display.setSegments(segments, 4);
  delay(1000); //?
}

#include <DHT.h>      // lib_deps = adafruit/DHT sensor library@^1.4.6
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(pinDHT, DHTTYPE);

#include "OneButton.h"          // lib_deps = mathertel/OneButton@^2.0.0
OneButton btn(pinButton, true); // click: send data, 2click: print log, long click: clear log

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
  TEMP_HUM,
  TIMER,
  CLOCK,
  MODE_COUNT
};
Mode mode = TEMP_HUM;

TimerItem timers[] = {
    TimerItem("COFF", new int[1]{60}, 1),
    TimerItem("EGGS", new int[1]{4 * 60}, 1),
    TimerItem("FREE", new int[1]{7 * 60}, 1), // "BLEJ"

    TimerItem("RESS", new int[1]{60}, 1), // rest between sets
    TimerItem("RESM", new int[1]{90}, 1),
    TimerItem("RESL", new int[1]{120}, 1),

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
    displayTimerName(timers[idxTimer = 0]);
    break;

  case CLOCK:
    displayNums(0, 0);
    // isClockCurrentTime = false;
    break;
  }
}

void setup()
{
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, LOW);
  // Serial.begin(115200);
  display.setBrightness(1);
  dht.begin();

  btn.attachClick([]()
                  {
                    switch (mode)
                    {
                      case TIMER:
                        if (++idxTimer >= sizeof(timers) / sizeof(timers[0]))
                          idxTimer = 0;
                        displayTimerName(timers[idxTimer]);
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
                          {
                            // Serial.print('.');
                            delay(1000);
                          }
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
                              display.clear();
                              delay(MS_ITV_SHORT);
                              displayNums(0, 0);
                              delay(MS_ITV_SHORT);
                              display.clear();
                              delay(MS_ITV_SHORT);
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
    else if (hum > 100)
      hum = 100;
    else
    {
      temp = temp + 0.5; // round to nearest int
      hum = hum + 0.5;   // round to nearest int
    }
    displayNums(temp, hum);
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
      displayTimerName(timers[idxTimer]);
    }
    else
      displayNums(remaining / 60, remaining % 60);
    break;
  }

  case CLOCK:
  {
    if (msClockStart == 0)
      break;
    int mm = (millis() - msClockStart) / 60000;
    int hh = mm / 60;
    mm = mm % 60;
    displayNums(hh, mm);
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

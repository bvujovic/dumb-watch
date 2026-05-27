#include <Arduino.h>

#if defined(CONFIG_IDF_TARGET_ESP32C3)
const byte pinClock = 4;   // TM1637 display CLK pin
const byte pinDio = 3;     // TM1637 display DIO pin
const byte pinDHT = 1;     // DHT sensor pin
const byte pinButton = 2;  // button pin
const byte pinBuzzer = 21; // buzzer pin
#elif defined(CONFIG_IDF_TARGET_ESP32)
const byte pinClock = 32;  // TM1637 display CLK pin
const byte pinDio = 33;    // TM1637 display DIO pin
const byte pinDHT = 25;    // DHT sensor pin
const byte pinButton = 14; // button pin
const byte pinBuzzer = 27; // buzzer pin
#endif

#include <TM1637Display.h>
TM1637Display display(pinClock, pinDio);

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
  default:
    return 0;
  }
}

void displayTimerName(const TimerItem &timer)
{
  uint8_t segments[4];
  for (int i = 0; i < 4; ++i)
    segments[i] = encodeCharToSegments(timer.name[i]);
  display.setSegments(segments, 4);
  delay(1000);
}

#include <DHT.h> // lib_deps = adafruit/DHT sensor library@^1.4.6
// #define DHTPIN 25     // DHT sensor pin
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(pinDHT, DHTTYPE);

#include "OneButton.h"          // lib_deps = mathertel/OneButton@^2.0.0
OneButton btn(pinButton, true); // click: send data, 2click: print log, long click: clear log

void buzz(int ms)
{
  digitalWrite(pinBuzzer, HIGH);
  delay(ms);
  digitalWrite(pinBuzzer, LOW);
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
    TimerItem("t1  ", new int[1]{1}, 1),
    TimerItem("  t2", new int[1]{2}, 1),
    TimerItem("TEST", new int[1]{1}, 1),
    TimerItem("EGGS", new int[3]{4, 4, 4}, 3),
    TimerItem("COFF", new int[2]{4, 4}, 2),
};
int idxTimer = 0;
ulong msTimerStart;

void nextMode()
{
  display.clear();
  msLastDisplayUpdate = 0;
  mode = static_cast<Mode>((mode + 1) % MODE_COUNT);

  switch (mode)
  {
  case TIMER:
    displayTimerName(timers[idxTimer = 0]);
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  display.setBrightness(1);
  dht.begin();
  pinMode(pinBuzzer, OUTPUT);

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
                    } });

  btn.attachDoubleClick([]()
                        { 
                          switch (mode)
                          {
                            case TIMER:
                              msTimerStart = millis();
                              break;
                          } });

  btn.attachLongPressStart([]()
                           { nextMode(); });
  msLastDisplayUpdate = millis();
}

void loop()
{
  delay(20);
  btn.tick();

  switch (mode)
  {
  case TEMP_HUM:
  {
    if (millis() - msLastDisplayUpdate < 5000)
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
    int remaining = timers[idxTimer].intervals[0] * 60 - (millis() - msTimerStart) / 1000;
    if (remaining < 0)
    {
      remaining = 0;
      msTimerStart = 0;
      buzz(1000);
    }
    displayNums(remaining / 60, remaining % 60);
    break;
  }

  case CLOCK:
  {
    break;
  }
  }
}

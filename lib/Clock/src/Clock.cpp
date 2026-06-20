#include "Clock.h"

Clock::Clock(Buzzer *buzzer)
{
  this->buzzer = buzzer;
}

void Clock::getTime()
{
  time(&now);             // read the current time
  localtime_r(&now, &ti); // update the structure tm with the current time
}

bool Clock::abortGetTimeIN()
{
  if (millis() > msStartGetTime + msMaxGetTime)
  {
    WiFi.mode(WIFI_OFF);
    isClockCurrentTime = false;
    msClockStart = millis();
    buzzer->buzz(MS_ITV_SHORT, 3);
    return true;
  }
  return false;
}

void Clock::getCurrentTime()
{
  // display.clear();
  msStartGetTime = millis();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_13dBm);

  struct NetCred
  {
    String ssid;
    String pass;
  };
  NetCred nets[] = {
      {WIFI_SSID, WIFI_PASS},
      {"POCO X3 Pro", "mile1234"},
  };
  int n = WiFi.scanNetworks();
  for (size_t i = 0; i < sizeof(nets) / sizeof(NetCred); i++)
  {
    // find an index of the first network with nets[i].ssid name
    for (int j = 0; j < n; j++)
      if (WiFi.SSID(j) == nets[i].ssid)
      {
        WiFi.begin(nets[i].ssid.c_str(), nets[i].pass.c_str());
        break;
      }
  }
  // WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    if (abortGetTimeIN())
      return;
  }
  configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TZ, 1);          // Set environment variable with your time zone
  tzset();
  while (ti.tm_year < (2025 - 1900))
  {
    delay(1000);
    getTime();
    if (abortGetTimeIN())
      return;
  }
  WiFi.mode(WIFI_OFF); // turn off wifi to save power, we don't need it anymore
  msClockStart = millis() - (ti.tm_hour * 3600 + ti.tm_min * 60 + ti.tm_sec) * 1000;
  lastBeepedMin = -1;
}

void Clock::start()
{
  if (!isClockCurrentTime)
  {
    if (!isPaused())
      msClockStart = millis();
    else
      msClockStart = millis() - (msClockPause - msClockStart);
    msClockPause = 0;
    lastBeepedMin = -1;
  }
}

void Clock::pause()
{
  if (!isClockCurrentTime)
  {
    msClockPause = millis();
  }
}

bool Clock::display()
{
  auto ms = !isPaused() ? millis() - msClockStart : msClockPause - msClockStart;
  // auto newTempSecs = ms / 1000;
  // if (newTempSecs != tempSecs)
  //   Serial.println(tempSecs = newTempSecs);
  int mm = ms / 60000;
  int hh = (mm / 60) % 24;
  mm = mm % 60;
  if (mm != lastBeepedMin && (mm % 5 == 0) && !(mm == 0 && lastBeepedMin == -1) && clockBeeps != NeverBeeps)
  {
    if (mm % 30 == 0) // 00 -> 2x LONG, 30 -> 1x LONG
      buzzer->buzz(MS_ITV_LONG, mm == 0 ? 2 : 1);
    else if (mm % 10 == 0) // 10, 40 -> 1x MED, 20, 50 -> 2x MED
    {
      if (clockBeeps != Every30min)
        buzzer->buzz(MS_ITV_MEDIUM, mm == 10 || mm == 40 ? 1 : 2);
    }
    else // 5, 15, 25, 35, 45, 55 -> 2x SHORT
      if (clockBeeps == Every5min)
        buzzer->buzz(MS_ITV_SHORT, 2);
    lastBeepedMin = mm;
  }
  hours = hh;
  minutes = mm;
  return true;
}

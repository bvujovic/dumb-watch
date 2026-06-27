#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <CredWiFi_mtsUmka.h>
#include <CredWiFi_HotSpot.h>
#include <CredWiFi_Vujovic.h>
#define MY_NTP_SERVER "rs.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#include "time.h"

#include "Buzzer.h"

// How often should clock beep: ev 5, 10, 30min
enum ClockBeeps
{
  Every5min = 5,   // Clock beeps every 5 minutes
  Every10min = 10, // Clock beeps every 10 minutes
  Every30min = 30, // Clock beeps every 30 minutes
  NeverBeeps = 0
};

class Clock
{
private:
  time_t now; // seconds since Epoch (1970) - UTC
  struct tm ti;
  ulong msClockStart = 0;
  ulong msClockPause = 1; // time (milliseconds) when the clock started
  int lastBeepedMin = -1;
  bool isClockCurrentTime = false;
  ulong msStartGetTime;
  const ulong msMaxGetTime = (ulong)15 * 1000;
  Buzzer *buzzer;
  int hours = 0, minutes = 0;
  ClockBeeps clockBeeps = Every5min;
  // ulong tempSecs = 0; // za testiranje racunanja protoka vremena; start/pause

  void getTime();
  // abort getting current time if it takes too long
  bool abortGetTimeIN();
  void abortGetTime();

public:
  Clock(Buzzer *buzzer);

  bool getClockCurrentTime() { return isClockCurrentTime; }
  void setClockCurrentTime(bool isCurrTime) { isClockCurrentTime = isCurrTime; }
  void toggleClockCurrentTime() { isClockCurrentTime = !isClockCurrentTime; }

  void getCurrentTime();

  /// @brief start clock, if it doesn't display current time
  void start();

  int getHours() { return hours; }
  int getMinutes() { return minutes; }

  ClockBeeps getClockBeeps() { return clockBeeps; }
  void setClockBeeps(ClockBeeps clockBeeps) { this->clockBeeps = clockBeeps; }

  bool isPaused() { return msClockPause != 0; }
  void pause();

  bool display();
  void beepIN();
};

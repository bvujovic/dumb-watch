#pragma once

#include <Arduino.h>
#include <WiFi.h>
// #include <CredWiFi.h>
#include <CredWiFi_HotSpot.h>
#define MY_NTP_SERVER "rs.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#include "time.h"

#include "Buzzer.h"

class Clock
{
private:
    time_t now; // seconds since Epoch (1970) - UTC
    struct tm ti;
    ulong msClockStart;
    int lastBeepedMin = -1;
    bool isClockCurrentTime = false;
    ulong msStartGetTime;
    const ulong msMaxGetTime = (ulong)15 * 1000;
    Buzzer *buzzer;
    int hours = 0, minutes = 0;

    void getTime();
    // abort getting current time if it takes too long
    bool abortGetTimeIN();

public:
    Clock(Buzzer *buzzer);

    bool getClockCurrentTime() { return isClockCurrentTime; }
    void setClockCurrentTime(bool isCurrTime) { isClockCurrentTime = isCurrTime; }
    void toggleClockCurrentTime() { isClockCurrentTime = !isClockCurrentTime; }

    void getCurrentTime();

    /// @brief start clock, if it doesn't display current time
    /// @return is clock started (only if it doesn't display current time)
    bool start();

    int getHours() { return hours; }
    int getMinutes() { return minutes; }

    bool display();
};

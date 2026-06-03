#include <CredWiFi.h> // WiFi credentials
#define MY_NTP_SERVER "rs.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#include <WiFi.h> // we need wifi to get internet access
#include <time.h> // for time() ctime()
time_t now;   // this are the seconds since Epoch (1970) - UTC
struct tm tm; // the structure tm holds time information in a more convenient way *

void getTime()
{
    time(&now);             // read the current time
    localtime_r(&now, &tm); // update the structure tm with the current time
}

void showTime()
{
    Serial.print(&tm, "%Y-%m-%d");
    Serial.println(&tm, " %H:%M:%S");

    // Serial.print("year:");
    // Serial.print(tm.tm_year + 1900); // years since 1900
    // Serial.print("\tmonth:");
    // Serial.print(tm.tm_mon + 1); // January = 0 (!)
    // Serial.print("\tday:");
    // Serial.print(tm.tm_mday); // day of month
    // Serial.print("\thour:");
    // Serial.print(tm.tm_hour); // hours since midnight 0-23
    // Serial.print("\tmin:");
    // Serial.print(tm.tm_min); // minutes after the hour 0-59
    // Serial.print("\tsec:");
    // Serial.print(tm.tm_sec); // seconds after the minute 0-61*
    // Serial.print("\twday");
    // Serial.print(tm.tm_wday); // days since Sunday 0-6
    // if (tm.tm_isdst == 1)     // Daylight Saving Time flag
    //     Serial.print("\tDST");
    // else
    //     Serial.print("\tstandard");
    // Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nNTP TZ DST - bare minimum");

    // start network
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_13dBm); // reduce power to save battery, we don't need more for NTP
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
    setenv("TZ", MY_TZ, 1);          // Set environment variable with your time zone
    tzset();

    Serial.println("Waiting for NTP time sync:");
    while (tm.tm_year < (2024 - 1900))
    {
        Serial.print(".");
        delay(1000);
        getTime();
    }
    Serial.println("Time synchronized!");
    showTime();
    WiFi.mode(WIFI_OFF); // turn off wifi to save power, we don't need it anymore
}

void loop()
{
    // getTime();
    // showTime();
    delay(5000); // dirty delay
}

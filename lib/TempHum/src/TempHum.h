#pragma once

#include <Arduino.h>
#include <DHT.h> // lib_deps = adafruit/DHT sensor library@^1.4.6
#include "Display.h"

class TempHum
{
private:
    DHT *_dht;
    Display *_display;
    bool _isBothTempHumDisplayed = true; // true - display temp:hum; false - display temp w/ decimals

public:
    TempHum(byte pinDHT, Display *display);
    
    // initialize DHT sensor
    void begin(); // initialize DHT sensor

    // if display temp:hum or display temp w/ decimals
    bool getBothTempHumDisplayed() { return _isBothTempHumDisplayed; }
    void setBothTempHumDisplayed(bool value) { _isBothTempHumDisplayed = value; }
    void toggleBothTempHumDisplayed() { _isBothTempHumDisplayed = !_isBothTempHumDisplayed; }

    // present data on display (temp:hum or just temp)
    void display();
};

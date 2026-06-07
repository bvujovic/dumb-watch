#include "TempHum.h"

TempHum::TempHum(byte pinDHT, Display* display)
{
    _dht = new DHT(pinDHT, DHT22);
    _display = display;
}

void TempHum::begin()
{
    _dht->begin();
}

void TempHum::display()
{
    float hum = _dht->readHumidity();
    float temp = _dht->readTemperature();
    if (isnan(hum) || isnan(temp))
        temp = hum = 0;
    if (hum > 100)
        hum = 100;
    if (getBothTempHumDisplayed())
    {
        temp += 0.5; // round to nearest int
        hum += 0.5;  // round to nearest int
        _display->nums(temp, hum);
    }
    else
    {
        auto intTemp = (int)temp;      // 25.46 -> 25
        auto decTemp = temp - intTemp; // 25.46 -> 0.46
        decTemp *= 100;
        _display->nums(intTemp, decTemp + 0.5);
    }
}

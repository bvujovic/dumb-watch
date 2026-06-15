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
        auto intTemp = (int)temp;      // 25.40 -> 25
        auto decTemp = (temp - intTemp) * 10; // 25.40 -> 0.4 -> 4
        char s[] = "123*";
        s[0] = '0' + (intTemp / 10) % 10;
        s[1] = '0' + intTemp % 10;
        s[2] = '0' + (int)decTemp;
        _display->string(s, true);
    }
}

#include <Arduino.h>

const byte pinWakeUp = 2;
RTC_DATA_ATTR int cntWakeUp = 0;

const byte pinLed = 8;

void ledOn(bool isOn) { digitalWrite(pinLed, isOn ? LOW : HIGH); }
void blinkLed(int itv)
{
    ledOn(true);
    delay(itv);
    ledOn(false);
    delay(itv);
}

void setup()
{
    pinMode(pinLed, OUTPUT);
    digitalWrite(pinLed, LOW);

    cntWakeUp++;

    for (int i = 0; i < cntWakeUp; i++)
        blinkLed(200);

    esp_deep_sleep_enable_gpio_wakeup(1ULL << pinWakeUp, ESP_GPIO_WAKEUP_GPIO_LOW);
    esp_deep_sleep_start();
}

void loop()
{
}

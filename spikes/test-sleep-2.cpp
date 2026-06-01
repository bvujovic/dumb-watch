#include <Arduino.h>

enum Mode
{
    MODE_1,
    MODE_2,
    MODE_COUNT
};

struct EspState
{
    int counter;
    Mode mode;
};
RTC_DATA_ATTR EspState state = {0, MODE_1};

const byte pinButton = 2; // button pin
const ulong msAwake = 5000;
ulong msLastUse;
const ulong msBlinkLong = 500;
const ulong msBlinkShort = 200;

const byte pinLed = 8;
void ledOn(bool isOn) { digitalWrite(pinLed, isOn ? LOW : HIGH); }
void blinkLed(int itv)
{
    ledOn(true);
    delay(itv);
    ledOn(false);
    delay(itv);
}

#include "OneButton.h"          // lib_deps = mathertel/OneButton@^2.0.0
OneButton btn(pinButton, true); // click: send data, 2click: print log, long click: clear log

void setup()
{
    pinMode(pinLed, OUTPUT);
    digitalWrite(pinLed, LOW);
    blinkLed(msBlinkLong);

    // click to increase counter
    btn.attachClick([]()
                    {
        state.counter++;
        for (int i = 0; i < state.counter; i++)
            blinkLed(msBlinkShort);
        msLastUse = millis(); });

        // long click to switch mode
    btn.attachLongPressStart([]()
                             { 
        state.mode = static_cast<Mode>((state.mode + 1) % MODE_COUNT);
        for (int i = 0; i < state.mode + 1; i++)
            blinkLed(msBlinkLong);
        msLastUse = millis(); });

    msLastUse = millis();
}

void loop()
{
    delay(20);
    btn.tick();

    // go to sleep if idle (no clicks) for a while
    if (millis() > msLastUse + msAwake)
    {
        blinkLed(msBlinkLong);
        esp_deep_sleep_enable_gpio_wakeup(1ULL << pinButton, ESP_GPIO_WAKEUP_GPIO_LOW);
        esp_deep_sleep_start();
    }
}

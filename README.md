# Dumb Watch
aka Vranićki Sat

![Test device](docs/test-device.jpg)

## Components
ESP32-C3 SuperMini, TM1637 display, buzzer, button

## Modes
- Temp/Hum: Display current temperature and humidity (data from DHT22 sensor)
- Timer: countdown from some given time (several hardcoded items - COFFee (1min), EGGS (4min)...)
- Clock: time goes forward; beeps on every 5/10 minutes; format HH:MM; start from 00:00 or displays current time

## TODO
- [ ] Code refactor
   - [ ] separate code into multiple files
   - [ ] use classes for different components (clock, timer, temp/hum sensor, display)
- [ ] Make device low power
   - [ ] temp/hum: read less often; sleep between reads
   - [ ] clock: sleep between display updates
   - [ ] auto turn off screen
- [ ] Maybe it would be better if 2 or 3 buttons are used instead of 1
   - TIMER: next/prev timer...
- [ ] Maybe it would be better if clicks would work like this:
   - click -> next item (mode, timer item...)
   - long click -> enter/start
   - double click -> exit/back
   - modes would display their names initially: TEHU (temp/hum), TIMR/TIME, CLCK/CLOC
- [ ] Improve component placement on the breadboard
   - [ ] Display should be horizontal (adjustable angle: solid wire glued to the side of the BB and row of 4 female pin headers or to display directly so that dupont wires can be used)
   - [ ] Make a place holder for Li-Ion battery
      - for "drone" batteries: metal profile (or couple of solid wires) glued to the bottom of the BB or just hot glue greased drone battery to the bottom of the BB. Make a room for boost converter (5V).

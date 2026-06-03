# Dumb Watch
aka Vranićki Sat

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
- [x] Clock
   - [x] Clock shouldn't beep as soon as it starts but it might be good to blink or something 
   so that user knows that it started
   - [x] Implement different beeps; copy idea/code from lib/TimeWatcher/src/TimeWatcher.cpp
- [ ] Timer
   - [x] TimerItem object should be initialized using seconds, not minutes.
   - [x] Decide if Timer should have one or more countdown times. Start with just one time per timer/name.
   - [ ] Click: if in countdown - pause/stop (maybe click when paused - back to timer's name); if not - next timer (display the name). Maybe double click should pause timer that's in progress.
- [ ] Make device low power
   - [ ] temp/hum: read less often; sleep between reads
   - [ ] clock: sleep between display updates
   - [ ] auto turn off screen
- [ ] Maybe it would be better if 2 or 3 buttons are used instead of 1
   - TIMER: next/prev timer...

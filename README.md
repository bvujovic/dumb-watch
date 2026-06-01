# Dumb Watch
aka Vranićki Sat

...

## Modes
- [x] Timer: countdown from some given time (couple of given start times - jaja (4min,6min,?min), kafa...)
- [ ] Clock: time goes forward; beeps on every 5/10 minutes; format HH:MM
   - [x] Start from 00:00
   - [ ] Display current time

## TODO
- [ ] Code refactor
   - [ ] separate code into multiple files
   - [ ] use classes for different components (clock, timer, temp/hum sensor, display)
- [ ] Clock
   - [ ] Clock shouldn't beep as soon as it starts but it might be good to blink or something 
   so that user knows that it started
   - [ ] Implement different beeps; copy idea/code from lib/TimeWatcher/src/TimeWatcher.cpp
- [ ] Timer
   - [x] TimerItem object should be initialized using seconds, not minutes.
   - [ ] Decide if Timer should have one or more countdown times. Start with just one time per timer/name.
- [ ] Make device less power hungry
   - [ ] temp/hum: read less often; sleep between reads
   - [ ] clock: sleep between display updates

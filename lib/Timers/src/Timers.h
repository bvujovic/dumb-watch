#pragma once

typedef unsigned long ulong; // #include <types.h>

class TimerItem
{
public:
  char name[4];
  int secInterval;

  TimerItem(const char nameIn[4], int min, int sec)
      : secInterval(min * 60 + sec)
  {
    for (int i = 0; i < 4; ++i)
      name[i] = nameIn[i];
  }
};

#include <vector>

class Timers
{
private:
  int idxTimer = 0;
  ulong msTimerStart;
  std::vector<TimerItem> timers;

public:
  Timers();

  void add(const TimerItem &ti);
  bool isEmpty() const { return timers.size() == 0; }
  const TimerItem &current() const { return timers[idxTimer]; }

  void moveToFirst();
  void moveToNext();

  void resetTime() { msTimerStart = 0; }
  void startTime(ulong ms) { msTimerStart = ms; }
  bool timeStarted() const { return msTimerStart > 0; }
  int secondsRemaining(ulong ms) const;
};

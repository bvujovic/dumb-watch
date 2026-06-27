#include "Timers.h"

Timers::Timers()
{
}

void Timers::add(const TimerItem &ti)
{
  timers.push_back(ti);
}

void Timers::moveToFirst()
{
  idxTimer = 0;
}

void Timers::moveToNext()
{
  // if (++idxTimer >= sizeof(timers) / sizeof(timers[0]))
  if (++idxTimer >= timers.size())
    idxTimer = 0;
}

int Timers::secondsRemaining(ulong ms) const
{
  if (isEmpty() || !timeStarted())
    return 123;
  else
    return current().secInterval - (ms - msTimerStart) / 1000;
}

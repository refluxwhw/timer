#ifndef TIMINGWHEELTIMER_H
#define TIMINGWHEELTIMER_H

#include "AbstractTimer.h"
#include <chrono>

class TWTimer : public AbstractTimer
{
    friend class TimingWheel;

public:
    TWTimer();
    ~TWTimer();
    virtual void start(uint32_t interval, OnTimerCB cb, Type type = Type::Circle);
    virtual void stop();

private:
    virtual void onTimer();

private:
    int m_vecIndex = -1;
    std::list<TWTimer*>::iterator m_itr;
    uint32_t m_interval = 0;          /// 触发间隔s

    OnTimerCB m_cb = nullptr;
};

#endif // TIMINGWHEELTIMER_H

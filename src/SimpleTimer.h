#ifndef SIMPLETIMER_H_
#define SIMPLETIMER_H_

/**
 * copy from http://www.cnblogs.com/gtarcoder/p/4924097.html
 */

#include<functional>
#include<chrono>
#include<thread>
#include<atomic>
#include<memory>
#include<mutex>
#include<condition_variable>
#include "AbstractTimer.h"

class SimpleTimer : public AbstractTimer
{
public:
    SimpleTimer();
    SimpleTimer(const SimpleTimer& t);
    ~SimpleTimer();

    virtual void start(uint32_t interval, OnTimerCB cb, Type type = Type::Circle);
    void stop();

private:
    std::atomic<bool> expired_;
    std::atomic<bool> try_to_expire_;
    std::mutex mutex_;
    std::condition_variable expired_cond_;

    std::thread* m_thread = nullptr;
};

#endif // SIMPLETIMER_H_

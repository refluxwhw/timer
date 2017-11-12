#include "SimpleTimer.h"


SimpleTimer::SimpleTimer()
    :expired_(true), try_to_expire_(false)
{}

SimpleTimer::SimpleTimer(const SimpleTimer &t)
{
    expired_ = t.expired_.load();
    try_to_expire_ = t.try_to_expire_.load();
}

SimpleTimer::~SimpleTimer()
{
    stop();
}

void SimpleTimer::start(uint32_t interval, OnTimerCB cb, AbstractTimer::Type type)
{
    if (expired_ == false) {
        return;
    }
    type_ = type;
    expired_ = false;
    try_to_expire_ = false;

    std::thread([this, interval, cb] () {
        while (!try_to_expire_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));

            if (!try_to_expire_) {
                cb();

                if (type_ == AbstractTimer::Once) {
                    try_to_expire_ = true;
                }
            }
        }

        {
            std::lock_guard<std::mutex> locker(mutex_);
            expired_ = true;
            expired_cond_.notify_one();
        }
    }).detach();
}

void SimpleTimer::stop()
{
    if (expired_) {
        return;
    }

    if (try_to_expire_) {
        return;
    }

    try_to_expire_ = true;

    {
        std::unique_lock<std::mutex> locker(mutex_);
        expired_cond_.wait(locker, [this]{ return expired_ == true; });
        if (expired_ == true) {
            try_to_expire_ = false;
        }
    }
}


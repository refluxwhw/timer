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
    stop();

    // if (0 == interval) {
    //     throw "timer interval can not be 0";
    // }

    if (expired_ == false) {
        return;
    }
    m_type = type;
    expired_ = false;
    try_to_expire_ = false;

    std::thread([this, interval, cb] () {
        std::mutex stop_mutex;
        while (!try_to_expire_) {
            std::unique_lock<std::mutex> locker(stop_mutex);
            if (std::cv_status::timeout == stop_cond_.wait_for(
                        locker, std::chrono::seconds(interval))) {
                cb();
                if (m_type == AbstractTimer::Type::Once) {
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
        stop_cond_.notify_one();

        std::unique_lock<std::mutex> locker(mutex_);
        expired_cond_.wait(locker, [this]{ return expired_ == true; });
        if (expired_ == true) {
            try_to_expire_ = false;
        }
    }
}


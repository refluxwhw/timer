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

#if 0
    std::chrono::steady_clock::time_point exp = std::chrono::steady_clock::now()
                                                + std::chrono::milliseconds(interval);
    m_thread = new std::thread([this, exp, interval, cb, type] () {
        std::this_thread::sleep_for(exp - std::chrono::steady_clock::now());
        cb();
        if (type == AbstractTimer::Type::Circle) {
            this->start(interval, cb, type);
        }
    });

#else

    if (expired_ == false) {
        return;
    }
    m_type = type;
    expired_ = false;
    try_to_expire_ = false;

    std::thread([this, interval, cb] () {
        while (!try_to_expire_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));

            if (!try_to_expire_) {
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
#endif
}

void SimpleTimer::stop()
{
#if 0

    if (m_thread != nullptr) {
        delete m_thread;
        m_thread = nullptr;
    }

#else
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
#endif
}


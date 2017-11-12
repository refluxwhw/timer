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

    virtual void start(uint32_t interval, OnTimerCB cb, Type type = Circle);
    void stop();

//    template<typename callable, class... arguments>
//    void SyncWait(int after, callable&& f, arguments&&... args)
//    {
//        std::function<typename std::result_of<callable(arguments...)>::type()> task
//                (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
//        std::this_thread::sleep_for(std::chrono::milliseconds(after));
//        task();
//    }

//    template<typename callable, class... arguments>
//    void AsyncWait(int after, callable&& f, arguments&&... args)
//    {
//        std::function<typename std::result_of<callable(arguments...)>::type()> task
//                (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
//
//        std::thread([after, task](){
//            std::this_thread::sleep_for(std::chrono::milliseconds(after));
//            task();
//        }).detach();
//    }

private:
    std::atomic<bool> expired_;
    std::atomic<bool> try_to_expire_;
    std::mutex mutex_;
    std::condition_variable expired_cond_;
};

#endif // SIMPLETIMER_H_

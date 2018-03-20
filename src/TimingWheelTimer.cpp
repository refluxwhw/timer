#include "TimingWheelTimer.h"

#include <list>
#include <vector>
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

#include "SingleTon.h"

using namespace std::chrono;


/**
 *  定时器要求异步执行任务    ----> 开辟独立的线程
 *  定时器要求能够启动和取消  ----> 提供安全的取消操作，使用互斥量和信号量
 *  定时器要求每个定时时刻到达的时候执行的任务要尽可能节省时间
 */

/// 如果时间轮大小一致
/// +----------+----+-------+-----+--------------+
/// | level    | 1  | 2     | ... | n            |
/// +----------+----+-------+-----+--------------+
/// | slot     | s  | s     | ... | s            |
/// +----------+----+-------+-----+--------------+
/// | max time | s  | s^2   | ... | s^n          |
/// +----------+----+-------+-----+--------------+

#define TW_SBIT 8
#define TW_NBIT 2
#define TW_SIZE (1<<TW_SBIT) // 时间轮大小为256
#define TW_NUM  (1<<TW_NBIT) // 时间轮层数为4

class TimingWheel
{
    SINGLETON(TimingWheel);

public:
    void addTimer(TWTimer* timer);
    void removeTimer(TWTimer* timer);
    void tick();
    bool moveUp(int n);

private:
    void loopThread();

private:
    typedef std::list<TWTimer*> TimerList;
    uint64_t m_checkTime;  /// 从开机到检查时间点的s数
    std::vector<TimerList> m_timers;
    steady_clock::time_point m_startTimePoint;

    std::atomic_bool m_stopFlag;
    std::thread* m_loopThread = nullptr;
};

SINGLETON_IMPL(TimingWheel);

/// ----------------------------------------------------------------------------
TimingWheel::TimingWheel()
{
    int wheelSize = TW_SIZE * TW_NUM;
    m_timers.resize(wheelSize);

    m_startTimePoint = steady_clock::now();
    m_checkTime = 0;

    m_loopThread = new std::thread(&TimingWheel::loopThread, this);
}

TimingWheel::~TimingWheel()
{
    m_stopFlag = true;
    m_loopThread->join();
    delete m_loopThread;
    m_loopThread = nullptr;

    /// 将所有 Timer 的位置设为 -1
    for (int i=0; i<TW_SIZE * TW_NUM; i++)
    {
        TimerList& tlist = m_timers[i];
        for (auto itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            TWTimer *timer = *itr;
            timer->m_vecIndex = -1;
        }
        tlist.clear();
    }
}

void TimingWheel::addTimer(TWTimer *timer)
{
    steady_clock::time_point now = steady_clock::now();
    seconds sec = duration_cast<seconds>(now - m_startTimePoint) + seconds(timer->m_interval);
    uint64_t idx = sec.count() - m_checkTime;

    // 得到所在时间轮的下标
    // 0:1~256; 1:256+1~256^2; x:256^x+1~256^(x+1);
    int offset = 0; // 得到所在时间轮
    uint64_t max = 1;
//    while ( !(idx < (max<<=TW_SBIT)) )
//    {
//        offset++;
//    }

    while ( true )
    {
        max *= 256;
        if (idx < max)
        {
            break;
        }
        offset++;
    }

    // 当前层时间轮，每个槽的大小为 256^n，
    // 下标应等于 当前检查时间在该层时间轮中的位置 + 值/槽大小
    int curIndex = (int)(m_checkTime >> (offset*TW_SBIT)) & (TW_SIZE-1);
    int arrIndex = (int)(idx >> (offset*TW_SBIT)) & (TW_SIZE-1);
    arrIndex += curIndex;
    arrIndex &= (TW_SIZE-1);
    int index = offset * TW_SIZE + arrIndex; // 在整个数组中的位置

    // 插入，记录下标信息，记录所在list的信息，便于删除
    TimerList& list = m_timers[index];
    list.push_back(timer);
    auto it = list.end();
    timer->m_vecIndex = index;
    timer->m_itr = --it;
}

void TimingWheel::removeTimer(TWTimer *timer)
{
    TimerList& list = m_timers[timer->m_vecIndex];
    list.erase(timer->m_itr);
}

void TimingWheel::tick()
{
    milliseconds msec = duration_cast<milliseconds>(steady_clock::now() - m_startTimePoint);
    uint64_t now = msec.count();

    while (m_checkTime*1000 <= now)
    {
        for (int i=0; i<TW_NUM; i++)
        {
            if (!moveUp(i))
            {
                break;
            }
        }

        /// FIXME: 在获取到一个槽的定时器时，其他线程调用了删除定时器，会导致删除错误。
        int index = m_checkTime & (TW_SIZE-1);

        TimerList& tlist = m_timers[index];
        TimerList temp;
        temp.splice(temp.end(), tlist);
        for (auto itr = temp.begin(); itr != temp.end(); ++itr)
        {
            TWTimer *timer = *itr;
            timer->onTimer();
        }

        m_checkTime += 1; // 每次增加 1s

        msec = duration_cast<milliseconds>(steady_clock::now() - m_startTimePoint);
        now = msec.count();
    }
}

bool TimingWheel::moveUp(int n) /// wheel index
{
    int index = (m_checkTime >> (n*TW_SBIT)) & (TW_SIZE-1);
    if ( 0 != index )
    {
        return false;
    }

    /// FIXME: 在获取到一个槽的定时器时，其他线程调用了删除定时器，会导致删除错误。
    TimerList& tlist = m_timers[n*TW_SIZE + index];
    TimerList temp;
    temp.splice(temp.end(), tlist);
    for (auto itr = temp.begin(); itr != temp.end(); ++itr)
    {
        addTimer(*itr);
    }

    return true;
}

void TimingWheel::loopThread()
{
    m_stopFlag = false;
    while (!m_stopFlag)
    {
        milliseconds ms = duration_cast<milliseconds>(steady_clock::now() - m_startTimePoint);
        uint64_t now = ms.count();

        if (now < m_checkTime*1000)
        {
            std::this_thread::sleep_for(milliseconds(m_checkTime*1000-now));
        }

        tick();
    }
}


/// ----------------------------------------------------------------------------

TWTimer::TWTimer()
{

}

TWTimer::~TWTimer()
{

}

void TWTimer::start(uint32_t interval, OnTimerCB cb, Type type)
{
    stop();

    //if (0 == interval) {
    //    throw "timer interval can not be 0";
    //}

    m_type = type;
    m_cb   = cb;
    m_interval = interval;

    TimingWheel::getInstance().addTimer(this);
}

void TWTimer::stop()
{
    if (m_vecIndex != -1)
    {
        TimingWheel::getInstance().removeTimer(this);
        m_vecIndex = -1;
    }
}

void TWTimer::onTimer()
{
    if (m_type == Type::Circle)
    {
        TimingWheel::getInstance().addTimer(this);
    }
    else
    {
        m_vecIndex = -1;
    }

    if (nullptr != m_cb)
    {
        m_cb();
    }
}

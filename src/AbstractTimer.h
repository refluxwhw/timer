#ifndef ABSTRACTTIMER_H
#define ABSTRACTTIMER_H

#include <functional>
#include <list>


#ifdef _WIN32
# include <Windows.h>
#else
# include <sys/time.h>
#endif

#define LOG_INFO(format, ...) do { \
    std::string _s_ = getCurTimeStr(); \
    printf("%s :", _s_.c_str()); \
    printf(format, ##__VA_ARGS__);\
    printf("\n");\
    } while (0)

static std::string getCurTimeStr()
{
    char buf[32] = {0};
#ifdef _WIN32
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    sprintf(buf,"%04u-%02u-%02u %02u:%02u:%02u.%03u",
            currentTime.wYear,currentTime.wMonth,currentTime.wDay,
            currentTime.wHour,currentTime.wMinute,currentTime.wSecond,
            currentTime.wMilliseconds);
#else
    struct timeval tv;
    if (0 == gettimeofday(&tv, NULL))
    {
        time_t timep = tv.tv_sec;
        struct tm result;
        localtime_r(&timep, &result);
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
                (1900+result.tm_year), (1+result.tm_mon), result.tm_mday,
                result.tm_hour, result.tm_min, result.tm_sec,
                tv.tv_usec / 1000);
    }
#endif
    return std::string(buf);
}

typedef std::function<void(void)> OnTimerCB;

class AbstractTimer
{
public:
    enum class Type {Once=0, Circle};

public:
    virtual ~AbstractTimer() {}
    /**
     * @brief start 启动定时器
     * @param interval: 定时时间，单位s
     * @param cb: 定时器触发时的回调函数
     * @param type: 单次触发还是循环触发
     */
    virtual void start(uint32_t interval, OnTimerCB cb, Type type = Type::Circle) = 0;
    /**
     * @brief stop 关闭定时器
     */
    virtual void stop() = 0;

    Type type() const { return m_type; }

protected:
    Type m_type = Type::Circle;
};

#endif // ABSTRACTTIMER_H

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include "../src/SimpleTimer.h"
#include "../src/TimingWheelTimer.h"

#ifdef _WIN32
# include <Windows.h>
#elif

#endif

using namespace std;


#define LOG_INFO(format, ...) do { \
    string _s_ = getCurTimeStr(); \
    printf("%s :", _s_.c_str()); \
    printf(format, ##__VA_ARGS__);\
    printf("\n");\
    } while (0)

std::string getCurTimeStr()
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




void EchoFunc(std::string&& s)
{
    LOG_INFO("echo: %s", s.c_str());
}

int main()
{
    //SimpleTimer t;
    TWTimer t;
    LOG_INFO("start once: 1s, sleep 6s");
    t.start(1, std::bind(EchoFunc, "once"), SimpleTimer::Type::Once);
    std::this_thread::sleep_for(std::chrono::seconds(6));

    LOG_INFO("start circle: 1s, sleep 7.5s");
    t.start(1, std::bind(EchoFunc, "circle"), SimpleTimer::Type::Circle);
    std::this_thread::sleep_for(std::chrono::milliseconds(7500));
    t.stop();
    LOG_INFO("timer stoped");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}

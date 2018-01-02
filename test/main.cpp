#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include "../src/SimpleTimer.h"
#include "../src/TimingWheelTimer.h"

using namespace std;
std::chrono::steady_clock::time_point g_start;

uint64_t now()
{
    auto now = std::chrono::duration_cast<std::chrono::microseconds>
               (std::chrono::steady_clock::now()-g_start);
    return now.count();
}

void EchoFunc(std::string&& s)
{
    std::cout << to_string(now()) << " echo : " << s << endl;
}

int main()
{
    //SimpleTimer t;
    TWTimer t;

    g_start = std::chrono::steady_clock::now();
    std::cout << to_string(now()) << endl;

    printf("start once: 2s, sleep 6s\n");
    t.start(2, std::bind(EchoFunc, "once"), SimpleTimer::Type::Once);
    std::this_thread::sleep_for(std::chrono::seconds(6));

    printf("start circle: 2s, sleep 7.5s\n");
    t.start(2, std::bind(EchoFunc, "circle"), SimpleTimer::Type::Circle);
    std::this_thread::sleep_for(std::chrono::milliseconds(7500));
    t.stop();
    printf("timer stoped\n");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << to_string(now()) << endl;

    return 0;
}

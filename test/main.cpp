#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include "../src/SimpleTimer.h"
#include "../src/TimingWheelTimer.h"

using namespace std;

void EchoFunc(std::string&& s)
{
    LOG_INFO("echo: %s", s.c_str());
}

int main()
{
    //SimpleTimer TWTimer;
    AbstractTimer* t = new SimpleTimer();
    LOG_INFO("start once: 1s, sleep 6s");
    t->start(1, std::bind(EchoFunc, "once"), SimpleTimer::Type::Once);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    t->stop();

    LOG_INFO("start circle: 1s, sleep 7.5s");
    t->start(1, std::bind(EchoFunc, "circle1"), SimpleTimer::Type::Circle);
    std::this_thread::sleep_for(std::chrono::milliseconds(7500));
    t->stop();
    LOG_INFO("timer stoped");

    delete t;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}

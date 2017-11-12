#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include "../src/SimpleTimer.h"

using namespace std;
std::chrono::steady_clock::time_point g_start;

uint64_t now()
{
    auto now = std::chrono::duration_cast<std::chrono::microseconds>
               (std::chrono::steady_clock::now()-g_start);
    return now.count();
}

void EchoFunc(std::string&& s){
    std::cout << to_string(now()) << " echo : " << s << endl;
}

int main() {
    SimpleTimer t;

    g_start = std::chrono::steady_clock::now();
    std::cout << to_string(now()) << endl;

    t.start(1000, std::bind(EchoFunc, "once"), SimpleTimer::Once);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << to_string(now()) << " timer has stoped automatically" << std::endl;

    t.start(1000, std::bind(EchoFunc,  "circle"), SimpleTimer::Circle);
    std::this_thread::sleep_for(std::chrono::milliseconds(3500));
    std::cout << to_string(now()) << " stop timer!" << std::endl;
    t.stop();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << to_string(now()) << endl;

    return 0;
}

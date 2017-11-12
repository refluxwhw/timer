#ifndef ABSTRACTTIMER_H
#define ABSTRACTTIMER_H

#include <functional>
#include <list>

typedef std::function<void(void)> OnTimerCB;

class AbstractTimer
{
public:
    enum Type {Once=0, Circle};

public:
    virtual void start(uint32_t interval, OnTimerCB cb, Type type = Circle) = 0;
    virtual void stop() = 0;

    Type type() const { return type_; }

protected:
    Type      type_     = Circle;
};

#endif // ABSTRACTTIMER_H

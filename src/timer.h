#pragma once

#include <iostream>
#include <chrono>

namespace NChat {

class TSimpleTimer
{
public:
    TSimpleTimer() {
        Reset();
    }

    void Reset() {
        Start_ = TClock::now();
    }

    double SecondsElapsed() const {
        return std::chrono::duration_cast<TSeconds>(TClock::now() - Start_).count();
    }

private:
    using TClock = std::chrono::high_resolution_clock;
    using TSeconds = std::chrono::duration<double, std::ratio<1>>;
    std::chrono::time_point<TClock> Start_;
};

}


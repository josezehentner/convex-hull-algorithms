#ifndef CORE_STOPWATCH_H
#define CORE_STOPWATCH_H

#include <chrono>

class Stopwatch {
public:
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

    void start();

    void stop();

    void reset();

    long long ns() const;

private:
    time_point start_tp{};
    std::chrono::nanoseconds accumulated{std::chrono::nanoseconds::zero()};
    bool running{false};
};

#endif

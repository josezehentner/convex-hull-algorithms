#include "stopwatch.h"

void Stopwatch::start() {
    accumulated = std::chrono::nanoseconds::zero();
    start_tp = clock::now();
    running = true;
}

void Stopwatch::stop() {
    if (!running) return;
    accumulated += std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - start_tp);
    running = false;
}

void Stopwatch::reset() {
    running = false;
    accumulated = std::chrono::nanoseconds::zero();
}

long long Stopwatch::ns() const {
    std::chrono::nanoseconds total = accumulated;
    if (running) {
        total += std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - start_tp);
    }
    return total.count();
}
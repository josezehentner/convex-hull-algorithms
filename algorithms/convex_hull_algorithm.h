#ifndef ALGORITHMS_CONVEX_HULL_ALGORITHM_H
#define ALGORITHMS_CONVEX_HULL_ALGORITHM_H

#include <vector>
#include "core/types.h"

class ConvexHullAlgorithm {
public:
    virtual ~ConvexHullAlgorithm() = default;

    virtual const char* name() const = 0;

    // set input points. indices in all outputs refer to this array
    virtual void reset(const std::vector<core::Point>& pts) = 0;

    // compute full hull. return indices in CCW order without repeating the first point
    virtual std::vector<int> run_full() = 0;

    // stepping API for the visualizer
    virtual void begin_stepping() = 0;
    virtual bool step() = 0; // return false when finished
    virtual const core::HullFrame& frame() const = 0;

    // optional reporting
    void report(long long ns, int hull_size) const;
};

#endif

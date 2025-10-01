#ifndef ALGO_CONVEX_HULL_ALGORITHM_H
#define ALGO_CONVEX_HULL_ALGORITHM_H

#include <vector>
#include <string>
#include "../core/types.h"

class ConvexHullAlgorithm {

    public:
        virtual ~ConvexHullAlgorithm() = default;

        virtual void reset(const std::vector<core::Point>& pts) = 0;

        virtual std::vector<int> run_full() = 0;

        virtual void begin_stepping() = 0;

        virtual bool step() = 0;

        virtual const core::HullFrame& frame() const = 0;

        virtual const char* name() const = 0;

        void report(const long long ns, const unsigned long size) const;
};

#endif

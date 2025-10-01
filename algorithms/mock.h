#ifndef ALGO_MOCK_H
#define ALGO_MOCK_H

#include "convex_hull_algorithm.h"
#include <vector>

class MockHull final : public ConvexHullAlgorithm {

    public:
        void reset(const std::vector<core::Point>& pts) override;

        std::vector<int> run_full() override;

        void begin_stepping() override;

        bool step() override;

        const core::HullFrame& frame() const override;

        const char* name() const override { return "Mock"; }

    private:
        std::vector<core::Point> points;
        core::HullFrame current;

        std::vector<core::HullFrame> script;
        std::size_t script_pos{0};

        int i_min_x{-1};
        int i_max_x{-1};
        int i_min_y{-1};
        int i_max_y{-1};

        void build_extremes();
        void build_script();
};

#endif

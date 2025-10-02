#ifndef ALGORITHMS_ANDREW_H
#define ALGORITHMS_ANDREW_H

#include "algorithms/convex_hull_algorithm.h"
#include "core/types.h"
#include <vector>

class AndrewAlgorithm final : public ConvexHullAlgorithm {
public:
    AndrewAlgorithm() = default;

    const char* name() const override { return "Andrew"; }

    void reset(const std::vector<core::Point>& pts) override;
    std::vector<int> run_full() override;

    void begin_stepping() override;
    bool step() override;
    const core::HullFrame& frame() const override { return fr_; };

private:
    std::vector<core::Point> points_;
    std::vector<int> order_;          // indices into points_, sorted by x then y

    // precomputed frames
    std::vector<core::HullFrame> frames_;
    std::size_t frame_pos_{0};
    core::HullFrame fr_{};

    // helpers
    static inline float cross(const core::Point& a, const core::Point& b, const core::Point& c) {
        const float abx = b.x - a.x;
        const float aby = b.y - a.y;
        const float acx = c.x - a.x;
        const float acy = c.y - a.y;
        return abx * acy - aby * acx;
    }
    static inline bool less_xy(const core::Point& a, const core::Point& b) {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        return a.y < b.y;
    }

    void build_sorted_order();
    static void append_no_dup(std::vector<int>& out, const std::vector<int>& part);

    // build the entire sequence of visual frames in frames_
    void build_frames();

    static core::HullFrame make_frame(const char* label,
                                      int a, int b, int c,
                                      const std::vector<int>& lower,
                                      const std::vector<int>& upper);
};

#endif

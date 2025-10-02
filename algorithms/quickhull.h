#ifndef ALGORITHMS_QUICKHULL_H
#define ALGORITHMS_QUICKHULL_H

#include "algorithms/convex_hull_algorithm.h"
#include "core/types.h"
#include <vector>

class Quickhull final : public ConvexHullAlgorithm {
public:
    Quickhull() = default;

    const char* name() const override { return "Quickhull"; }

    void reset(const std::vector<core::Point>& pts) override;
    std::vector<int> run_full() override;

    void begin_stepping() override;
    bool step() override;
    const core::HullFrame& frame() const override { return fr_; }

private:
    std::vector<core::Point> points_;

    // precomputed frames for the visual player
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
    static inline float tri_area2(const core::Point& a, const core::Point& b, const core::Point& c) {
        return std::abs(cross(a, b, c));
    }

    int leftmost_index() const;
    int rightmost_index() const;

    // build the final hull in CCW order without repeating endpoints
    std::vector<int> build_hull_ccw() const;

    // frame building
    void build_frames();
    static core::HullFrame make_frame(const char* label,
                                      int a, int b, int c,
                                      const std::vector<int>& upper_chain,
                                      const std::vector<int>& lower_chain);

    // quickhull chain builder used by both run full and frames
    static void chain_ccw(const std::vector<core::Point>& pts,
                          int a, int b,
                          const std::vector<int>& candidates, // indices
                          std::vector<int>& out);              // appends from a to b excluding b

    static void chain_ccw_with_frames(const std::vector<core::Point>& pts,
                                      int a, int b,
                                      const std::vector<int>& candidates, // indices
                                      std::vector<int>& upper_chain,       // or lower depending on call
                                      std::vector<int>& lower_chain,       // the other side, only for composing frames
                                      std::vector<core::HullFrame>& frames);
};

#endif

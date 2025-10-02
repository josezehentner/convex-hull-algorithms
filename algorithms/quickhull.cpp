#include "algorithms/quickhull.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>


using core::Point;

void Quickhull::reset(const std::vector<Point>& pts) {
    points_ = pts;
    frames_.clear();
    frame_pos_ = 0;
    fr_ = core::HullFrame{};
}

int Quickhull::leftmost_index() const {
    if (points_.empty()) return -1;
    int idx = 0;
    for (int i = 1; i < static_cast<int>(points_.size()); ++i) {
        if (points_[i].x < points_[idx].x ||
            (points_[i].x == points_[idx].x && points_[i].y < points_[idx].y)) {
            idx = i;
        }
    }
    return idx;
}

int Quickhull::rightmost_index() const {
    if (points_.empty()) return -1;
    int idx = 0;
    for (int i = 1; i < static_cast<int>(points_.size()); ++i) {
        if (points_[i].x > points_[idx].x ||
            (points_[i].x == points_[idx].x && points_[i].y > points_[idx].y)) {
            idx = i;
        }
    }
    return idx;
}

void Quickhull::chain_ccw(const std::vector<Point>& pts,
                          int a, int b,
                          const std::vector<int>& candidates,
                          std::vector<int>& out) {
    // find farthest from segment ab among points left of ab
    int far = -1;
    float best = 0.0f;
    for (int idx : candidates) {
        float area2 = cross(pts[a], pts[b], pts[idx]);
        if (area2 > 0.0f) {
            float val = std::abs(area2); // distance proxy
            if (val > best) { best = val; far = idx; }
        }
    }

    if (far == -1) {
        // no point strictly left of ab, fix a
        out.push_back(a);
        return;
    }

    // split candidates into the two subproblems
    std::vector<int> left_ac;
    std::vector<int> left_cb;
    left_ac.reserve(candidates.size());
    left_cb.reserve(candidates.size());
    for (int idx : candidates) {
        if (idx == far) continue;
        float s1 = cross(pts[a], pts[far], pts[idx]);
        float s2 = cross(pts[far], pts[b], pts[idx]);
        if (s1 > 0.0f) left_ac.push_back(idx);
        else if (s2 > 0.0f) left_cb.push_back(idx);
        // collinear or right of both halves are ignored
    }

    chain_ccw(pts, a, far, left_ac, out);
    chain_ccw(pts, far, b, left_cb, out);
}

std::vector<int> Quickhull::build_hull_ccw() const {
    std::vector<int> hull;
    if (points_.empty()) return hull;
    if (points_.size() == 1) { hull.push_back(0); return hull; }

    int L = leftmost_index();
    int R = rightmost_index();
    if (L == -1 || R == -1 || L == R) {
        hull.push_back(L == -1 ? 0 : L);
        return hull;
    }

    std::vector<int> above;
    std::vector<int> below;
    above.reserve(points_.size());
    below.reserve(points_.size());
    for (int i = 0; i < static_cast<int>(points_.size()); ++i) {
        if (i == L || i == R) continue;
        float s = cross(points_[L], points_[R], points_[i]);
        if (s > 0.0f) above.push_back(i);
        else if (s < 0.0f) below.push_back(i);
        // collinear with LR are ignored, endpoints carry that edge
    }

    std::vector<int> top;
    std::vector<int> bot;
    chain_ccw(points_, L, R, above, top); // L to R without R
    chain_ccw(points_, R, L, below, bot); // R to L without L

    hull.reserve(top.size() + bot.size());
    hull.insert(hull.end(), top.begin(), top.end());
    hull.insert(hull.end(), bot.begin(), bot.end());
    return hull;
}

std::vector<int> Quickhull::run_full() {
    return build_hull_ccw();
}

core::HullFrame Quickhull::make_frame(const char* label,
                                      int a, int b, int c,
                                      const std::vector<int>& upper,
                                      const std::vector<int>& lower) {
    core::HullFrame f{};
    f.active_a = a;
    f.active_b = b;
    f.active_c = c;
    f.label = label;
    f.hull_indices.clear();
    f.hull_indices.reserve(upper.size() + lower.size());
    // upper carries L to R path without R
    // lower carries R to L path without L
    f.hull_indices.insert(f.hull_indices.end(), upper.begin(), upper.end());
    f.hull_indices.insert(f.hull_indices.end(), lower.begin(), lower.end());
    return f;
}

void Quickhull::chain_ccw_with_frames(const std::vector<Point>& pts,
                                      int a, int b,
                                      const std::vector<int>& candidates,
                                      std::vector<int>& upper_chain,
                                      std::vector<int>& lower_chain,
                                      std::vector<core::HullFrame>& frames) {
    // find farthest on the left of ab
    int far = -1;
    float best = 0.0f;
    for (int idx : candidates) {
        float area2 = cross(pts[a], pts[b], pts[idx]);
        if (area2 > 0.0f) {
            float val = std::abs(area2);
            if (val > best) { best = val; far = idx; }
        }
    }

    if (far == -1) {
        // fix a into whichever chain we are building
        upper_chain.push_back(a); // caller controls which vector is passed here
        frames.push_back(make_frame("Fix edge", a, b, -1, upper_chain, lower_chain));
        return;
    }

    frames.push_back(make_frame("Farthest from edge", a, b, far, upper_chain, lower_chain));

    std::vector<int> left_ac;
    std::vector<int> left_cb;
    left_ac.reserve(candidates.size());
    left_cb.reserve(candidates.size());
    for (int idx : candidates) {
        if (idx == far) continue;
        float s1 = cross(pts[a], pts[far], pts[idx]);
        float s2 = cross(pts[far], pts[b], pts[idx]);
        if (s1 > 0.0f) left_ac.push_back(idx);
        else if (s2 > 0.0f) left_cb.push_back(idx);
    }

    chain_ccw_with_frames(pts, a,   far, left_ac, upper_chain, lower_chain, frames);
    chain_ccw_with_frames(pts, far, b,   left_cb, upper_chain, lower_chain, frames);
}

void Quickhull::build_frames() {
    frames_.clear();

    if (points_.empty()) {
        core::HullFrame f{};
        f.label = "No points";
        frames_.push_back(std::move(f));
        return;
    }
    if (points_.size() == 1) {
        core::HullFrame f{};
        f.label = "Single point";
        f.hull_indices = {0};
        frames_.push_back(std::move(f));
        return;
    }

    int L = leftmost_index();
    int R = rightmost_index();
    if (L == -1 || R == -1 || L == R) {
        core::HullFrame f{};
        f.label = "Degenerate set";
        if (L >= 0) f.hull_indices = {L};
        frames_.push_back(std::move(f));
        return;
    }

    std::vector<int> above;
    std::vector<int> below;
    above.reserve(points_.size());
    below.reserve(points_.size());
    for (int i = 0; i < static_cast<int>(points_.size()); ++i) {
        if (i == L || i == R) continue;
        float s = cross(points_[L], points_[R], points_[i]);
        if (s > 0.0f)      above.push_back(i);
        else if (s < 0.0f) below.push_back(i);
    }

    frames_.push_back(make_frame("Split by LR", L, R, -1, {}, {}));

    // build top chain from L to R
    std::vector<int> top;
    std::vector<int> bot;
    chain_ccw_with_frames(points_, L, R, above, top, bot, frames_);

    frames_.push_back(make_frame("Switch to lower", R, L, -1, top, bot));

    // build bottom chain from R to L
    // reuse the same helper, but pass the live chain first
    chain_ccw_with_frames(points_, R, L, below, bot, top, frames_);

    // final frame with the composed hull, chains are already without duplicate endpoints
    core::HullFrame done = make_frame("Done", -1, -1, -1, top, bot);
    frames_.push_back(std::move(done));
}


void Quickhull::begin_stepping() {
    build_frames();
    frame_pos_ = 0;
    fr_ = frames_.front();
}

bool Quickhull::step() {
    if (frames_.empty()) return false;
    if (frame_pos_ + 1 >= frames_.size()) return false;
    ++frame_pos_;
    fr_ = frames_[frame_pos_];
    return frame_pos_ + 1 < frames_.size();
}

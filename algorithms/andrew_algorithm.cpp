#include "andrew_algorithm.h"
#include <algorithm>
#include <iostream>

using core::Point;

void AndrewAlgorithm::reset(const std::vector<Point>& pts) {
    points_ = pts;
    order_.clear();
    frames_.clear();
    frame_pos_ = 0;
    fr_ = core::HullFrame{};
}

void AndrewAlgorithm::build_sorted_order() {
    const int n = static_cast<int>(points_.size());
    order_.resize(n);
    for (int i = 0; i < n; ++i) order_[i] = i;

    std::sort(order_.begin(), order_.end(), [&](int i, int j) {
        return less_xy(points_[i], points_[j]);
    });

    // remove exact duplicates
    std::vector<int> uniq;
    uniq.reserve(order_.size());
    for (std::size_t k = 0; k < order_.size(); ++k) {
        if (k == 0) { uniq.push_back(order_[k]); continue; }
        const Point& p = points_[order_[k]];
        const Point& q = points_[uniq.back()];
        if (!(p.x == q.x && p.y == q.y)) uniq.push_back(order_[k]);
    }
    order_.swap(uniq);
}

std::vector<int> AndrewAlgorithm::run_full() {
    build_sorted_order();
    const int m = static_cast<int>(order_.size());
    if (m == 0) return {};
    if (m == 1) return {order_[0]};

    const int L = order_.front();
    const int R = order_.back();

    // lower chain from L to R
    std::vector<int> lower;
    lower.reserve(m);
    for (int idx : order_) {
        while (static_cast<int>(lower.size()) >= 2) {
            int a = lower[static_cast<int>(lower.size()) - 2];
            int b = lower[static_cast<int>(lower.size()) - 1];
            if (cross(points_[a], points_[b], points_[idx]) <= 0.0f) lower.pop_back();
            else break;
        }
        lower.push_back(idx);
    }
    // drop R to avoid duplicate when concatenating
    if (!lower.empty()) lower.pop_back();

    // mark lower indices to skip during upper, except extremes
    std::vector<char> in_lower(points_.size(), 0);
    for (int id : lower) in_lower[id] = 1;

    // upper chain from R to L, skip any index already in lower, allow L
    std::vector<int> upper;
    upper.reserve(m);
    for (int k = m - 1; k >= 0; --k) {
        int idx = order_[k];
        if (in_lower[idx] && idx != L) continue; // skip reused interior points

        while (static_cast<int>(upper.size()) >= 2) {
            int a = upper[static_cast<int>(upper.size()) - 2];
            int b = upper[static_cast<int>(upper.size()) - 1];
            if (cross(points_[a], points_[b], points_[idx]) <= 0.0f) upper.pop_back();
            else break;
        }
        upper.push_back(idx);
    }
    // drop L to avoid duplicate when concatenating
    if (!upper.empty()) upper.pop_back();

    std::vector<int> hull;
    hull.reserve(lower.size() + upper.size());
    hull.insert(hull.end(), lower.begin(), lower.end());
    hull.insert(hull.end(), upper.begin(), upper.end());
    return hull;
}


core::HullFrame AndrewAlgorithm::make_frame(const char* label,
                                            int a, int b, int c,
                                            const std::vector<int>& lower,
                                            const std::vector<int>& upper) {
    core::HullFrame f{};
    f.active_a = a;
    f.active_b = b;
    f.active_c = c;
    f.label = label;

    // Do not drop endpoints here. build_frames already removes duplicates
    // when switching from lower to upper and when finishing upper.
    f.hull_indices.clear();
    f.hull_indices.reserve(lower.size() + upper.size());
    f.hull_indices.insert(f.hull_indices.end(), lower.begin(), lower.end());
    f.hull_indices.insert(f.hull_indices.end(), upper.begin(), upper.end());
    return f;
}

void AndrewAlgorithm::build_frames() {
    frames_.clear();

    build_sorted_order();
    const int m = static_cast<int>(order_.size());
    if (m == 0) {
        core::HullFrame f{};
        f.label = "No points";
        frames_.push_back(std::move(f));
        return;
    }
    if (m == 1) {
        core::HullFrame f{};
        f.label = "Single point";
        f.hull_indices = {order_[0]};
        frames_.push_back(std::move(f));
        return;
    }

    const int L = order_.front();
    const int R = order_.back();

    std::vector<int> lower;
    std::vector<int> upper;
    lower.reserve(m);
    upper.reserve(m);

    frames_.push_back(make_frame("Start lower chain", -1, -1, -1, lower, upper));

    // lower chain
    for (int idx : order_) {
        while (static_cast<int>(lower.size()) >= 2) {
            int a = lower[static_cast<int>(lower.size()) - 2];
            int b = lower[static_cast<int>(lower.size()) - 1];
            if (cross(points_[a], points_[b], points_[idx]) <= 0.0f) {
                frames_.push_back(make_frame("Remove from lower", a, b, idx, lower, upper));
                lower.pop_back();
            } else {
                break;
            }
        }
        lower.push_back(idx);
        int a = static_cast<int>(lower.size()) >= 2 ? lower[lower.size() - 2] : -1;
        int b = static_cast<int>(lower.size()) >= 1 ? lower[lower.size() - 1] : -1;
        frames_.push_back(make_frame("Add to lower", a, b, -1, lower, upper));
    }

    frames_.push_back(make_frame("Start upper chain", -1, -1, -1, lower, upper));

    // drop R to avoid duplicate when concatenating
    if (!lower.empty()) lower.pop_back();

    // mark lower indices so upper pass skips them, except L
    std::vector<char> in_lower(points_.size(), 0);
    for (int id : lower) in_lower[id] = 1;

    // upper chain, iterate reversed sorted order, skip reused interior points
    for (int t = m - 1; t >= 0; --t) {
        int idx = order_[t];
        if (in_lower[idx] && idx != L) continue;

        while (static_cast<int>(upper.size()) >= 2) {
            int a = upper[static_cast<int>(upper.size()) - 2];
            int b = upper[static_cast<int>(upper.size()) - 1];
            if (cross(points_[a], points_[b], points_[idx]) <= 0.0f) {
                frames_.push_back(make_frame("Remove from upper", a, b, idx, lower, upper));
                upper.pop_back();
            } else {
                break;
            }
        }
        upper.push_back(idx);
        int a = static_cast<int>(upper.size()) >= 2 ? upper[upper.size() - 2] : -1;
        int b = static_cast<int>(upper.size()) >= 1 ? upper[upper.size() - 1] : -1;
        frames_.push_back(make_frame("Add to upper", a, b, -1, lower, upper));
    }

    // drop L to avoid duplicate when concatenating
    if (!upper.empty()) upper.pop_back();

    core::HullFrame done = make_frame("Done", -1, -1, -1, lower, upper);
    frames_.push_back(std::move(done));
}


void AndrewAlgorithm::begin_stepping() {
    build_frames();
    frame_pos_ = 0;
    fr_ = frames_.front();
}

bool AndrewAlgorithm::step() {
    if (frames_.empty()) return false;
    if (frame_pos_ + 1 >= frames_.size()) return false;
    ++frame_pos_;
    fr_ = frames_[frame_pos_];
    return frame_pos_ + 1 < frames_.size();
}

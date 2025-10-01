#include "mock.h"
#include <algorithm>
#include <unordered_set>

using core::Point;
using core::HullFrame;
using core::StepKind;

void MockHull::reset(const std::vector<Point>& pts) {
    points = pts;
    current = {};
    script.clear();
    script_pos = 0;
    i_min_x = i_max_x = i_min_y = i_max_y = -1;
}

static int first_index_or_neg1(const std::vector<Point>& pts, bool pick_min, bool on_x) {
    if (pts.empty()) return -1;
    int best = 0;
    for (int i = 1; i < static_cast<int>(pts.size()); ++i) {
        if (on_x) {
            if (pick_min) {
                if (pts[i].x < pts[best].x || (pts[i].x == pts[best].x && pts[i].y < pts[best].y)) best = i;
            } else {
                if (pts[i].x > pts[best].x || (pts[i].x == pts[best].x && pts[i].y > pts[best].y)) best = i;
            }
        } else {
            if (pick_min) {
                if (pts[i].y < pts[best].y || (pts[i].y == pts[best].y && pts[i].x < pts[best].x)) best = i;
            } else {
                if (pts[i].y > pts[best].y || (pts[i].y == pts[best].y && pts[i].x > pts[best].x)) best = i;
            }
        }
    }
    return best;
}

void MockHull::build_extremes() {
    i_min_x = first_index_or_neg1(points, true,  true);
    i_max_x = first_index_or_neg1(points, false, true);
    i_min_y = first_index_or_neg1(points, true,  false);
    i_max_y = first_index_or_neg1(points, false, false);
}

std::vector<int> MockHull::run_full() {
    build_extremes();
    std::vector<int> hull;
    std::unordered_set<int> seen;

    auto push_unique = [&](int idx) {
        if (idx < 0) return;
        if (!seen.contains(idx)) { hull.push_back(idx); seen.insert(idx); }
    };

    push_unique(i_min_x);
    push_unique(i_max_x);
    push_unique(i_max_y);
    push_unique(i_min_y);

    return hull;
}

void MockHull::begin_stepping() {
    build_extremes();
    build_script();
    script_pos = 0;
    current = script.empty() ? HullFrame{} : script.front();
}

bool MockHull::step() {
    if (script_pos + 1 >= script.size()) {
        current.kind = StepKind::Done;
        return false;
    }
    ++script_pos;
    current = script[script_pos];
    return true;
}

const HullFrame& MockHull::frame() const {
    return current;
}

void MockHull::build_script() {
    script.clear();
    HullFrame f;

    f = {};
    f.kind = StepKind::Start;
    f.active_a = i_min_x;
    f.active_b = i_max_x;
    f.label = "Start with two extreme points on x";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::PickPivot;
    f.active_c = i_min_y;
    f.label = "Pick bottom extreme as next pivot";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::AddToHull;
    f.hull_indices = { i_min_x };
    f.label = "Add left extreme to hull";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::ConsiderEdge;
    f.active_a = i_min_x;
    f.active_b = i_min_y;
    f.label = "Consider edge left to bottom";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::AddToHull;
    f.hull_indices = { i_min_x, i_min_y };
    f.label = "Confirm edge left to bottom";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::Advance;
    f.active_a = i_min_y;
    f.active_b = i_max_x;
    f.active_c = i_max_y;
    f.label = "Advance toward right and top extremes";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::AddToHull;
    f.hull_indices = { i_min_x, i_min_y, i_max_x };
    f.label = "Add right extreme";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::AddToHull;
    f.hull_indices = { i_min_x, i_min_y, i_max_x, i_max_y };
    f.label = "Add top extreme";
    script.push_back(f);

    f = script.back();
    f.kind = StepKind::Done;
    f.label = "Mock playback done";
    script.push_back(f);
}

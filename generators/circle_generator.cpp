#include "circle_generator.h"
#include <cmath>

using core::Point;

std::vector<Point> CircleGenerator::generate(std::size_t n, float w, float h) {
    std::vector<Point> pts;
    if (n == 0) return pts;
    pts.reserve(n);

    const float cx = h * 0.5f;
    const float cy = h * 0.5f;
    const float r  = 0.45f * std::min(w, h);
    const float two_pi = 6.2831853071795864769f;

    for (std::size_t i = 0; i < n; ++i) {
        float t = two_pi * static_cast<float>(i) / static_cast<float>(n);
        Point p{};
        p.x = cx + r * std::cos(t);
        p.y = cy + r * std::sin(t);
        p.id = static_cast<int>(pts.size());
        pts.push_back(p);
    }
    return pts;
}

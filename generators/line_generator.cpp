#include "line_generator.h"

using core::Point;

std::vector<Point> LineGenerator::generate(std::size_t n, float w, float h) {
    std::vector<Point> pts;
    if (n == 0) return pts;
    pts.reserve(n);

    const float m = 0.05f * std::min(w, h);
    const float x0 = m;
    const float y0 = h * 0.5f;
    const float x1 = w - m;
    const float y1 = h * 0.5f;

    for (std::size_t i = 0; i < n; ++i) {
        float t = (n == 1) ? 0.0f : static_cast<float>(i) / static_cast<float>(n - 1);
        Point p{};
        p.x = x0 + t * (x1 - x0);
        p.y = y0 + t * (y1 - y0);
        p.id = static_cast<int>(pts.size());
        pts.push_back(p);
    }
    return pts;
}

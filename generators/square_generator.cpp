#include "square_generator.h"
#include <algorithm>

using core::Point;

std::vector<Point> SquareGenerator::generate(std::size_t n, float w, float h) {
    std::vector<Point> pts;
    if (n == 0) return pts;
    pts.reserve(n);

    const float s  = 0.9f * std::min(w, h);
    const float cx = h * 0.5f;
    const float cy = h * 0.5f;
    const float left = cx - s * 0.5f;
    const float top  = cy - s * 0.5f;
    const float perim = 4.0f * s;

    for (std::size_t i = 0; i < n; ++i) {
        float d = perim * static_cast<float>(i) / static_cast<float>(n);
        float x, y;
        if (d < s) {
            x = left + d;
            y = top;
        } else if (d < 2.0f * s) {
            d -= s;
            x = left + s;
            y = top + d;
        } else if (d < 3.0f * s) {
            d -= 2.0f * s;
            x = left + s - d;
            y = top + s;
        } else {
            d -= 3.0f * s;
            x = left;
            y = top + s - d;
        }
        Point p{};
        p.x = x;
        p.y = y;
        p.id = static_cast<int>(pts.size());
        pts.push_back(p);
    }
    return pts;
}

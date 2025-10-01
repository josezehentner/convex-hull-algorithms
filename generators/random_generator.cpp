#include "random_generator.h"
#include <random>
#include <algorithm>
#include <cmath>

using core::Point;

std::vector<Point> RandomGenerator::generate(std::size_t n, float w, float h) {
    if (n < 4) n = 4;

    std::random_device rd;
    std::mt19937 rng(rd());

    auto cells = static_cast<std::size_t>(std::sqrt(static_cast<double>(n)));
    if (cells < 4) cells = 4;
    float cw = w / static_cast<float>(cells);
    float ch = h / static_cast<float>(cells);

    std::uniform_real_distribution<float> jx(0.15f * cw, 0.85f * cw);
    std::uniform_real_distribution<float> jy(0.15f * ch, 0.85f * ch);

    std::vector<Point> pts;
    pts.reserve(n);

    std::vector<std::pair<int,int>> order;
    order.reserve(cells * cells);
    for (int r = 0; r < static_cast<int>(cells); ++r)
        for (int c = 0; c < static_cast<int>(cells); ++c)
            order.emplace_back(r, c);
    std::shuffle(order.begin(), order.end(), rng);

    for (std::size_t k = 0; k < order.size() && pts.size() < n; ++k) {
        double r = order[k].first;
        double c = order[k].second;
        double x = c * cw + jx(rng);
        double y = r * ch + jy(rng);

        if (x < 0) x = 0;
        if (x > w) x = w;
        if (y < 0) y = 0;
        if (y > h) y = h;

        Point p{};
        p.x = x;
        p.y = y;
        p.id = static_cast<int>(pts.size());
        pts.push_back(p);
    }

    std::uniform_real_distribution<float> ux(0.0f, w);
    std::uniform_real_distribution<float> uy(0.0f, h);
    while (pts.size() < n) {
        Point p{};
        p.x = ux(rng);
        p.y = uy(rng);
        p.id = static_cast<int>(pts.size());
        pts.push_back(p);
    }

    return pts;
}

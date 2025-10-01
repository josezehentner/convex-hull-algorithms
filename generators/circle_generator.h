#ifndef GENERATORS_CIRCLE_GENERATOR_H
#define GENERATORS_CIRCLE_GENERATOR_H

#include "point_generator.h"

class CircleGenerator final : public PointGenerator {
public:
    const char* name() const override { return "Circle"; }
    std::vector<core::Point> generate(std::size_t n, float w, float h) override;
};

#endif

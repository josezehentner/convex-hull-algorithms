#ifndef GENERATORS_SQUARE_GENERATOR_H
#define GENERATORS_SQUARE_GENERATOR_H

#include "point_generator.h"

class SquareGenerator final : public PointGenerator {
public:
    const char* name() const override { return "Square"; }
    std::vector<core::Point> generate(std::size_t n, float w, float h) override;
};

#endif

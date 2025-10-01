#ifndef GENERATORS_RANDOM_GENERATOR_H
#define GENERATORS_RANDOM_GENERATOR_H

#include "point_generator.h"

class RandomGenerator final : public PointGenerator {
public:
    const char* name() const override { return "Random"; }
    std::vector<core::Point> generate(std::size_t n, float w, float h) override;
};

#endif

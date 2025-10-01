#ifndef GENERATORS_LINE_GENERATOR_H
#define GENERATORS_LINE_GENERATOR_H

#include "point_generator.h"

class LineGenerator final : public PointGenerator {
public:
    const char* name() const override { return "Line"; }
    std::vector<core::Point> generate(std::size_t n, float w, float h) override;
};

#endif

#ifndef GENERATORS_POINT_GENERATOR_H
#define GENERATORS_POINT_GENERATOR_H

#include <vector>
#include "core/types.h"

class PointGenerator {
public:
    virtual ~PointGenerator() = default;
    virtual const char* name() const = 0;
    virtual std::vector<core::Point> generate(std::size_t n, float w, float h) = 0;
};

#endif

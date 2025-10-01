#include <iostream>
#include "convex_hull_algorithm.h"

void ConvexHullAlgorithm::report(const long long ns, const unsigned long size) const {
    std::cout << name() << ": " << ns << "ns," << " hull size: " << size << std::endl;
}

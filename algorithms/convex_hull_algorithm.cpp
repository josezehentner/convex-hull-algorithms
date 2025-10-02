#include <iostream>
#include "convex_hull_algorithm.h"

void ConvexHullAlgorithm::report(long long ns, int hull_size) const {
    std::cout << name() << ": " << ns << "ns," << " hull size: " << hull_size << std::endl;
}

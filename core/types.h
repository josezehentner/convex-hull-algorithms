#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <vector>
#include <string>

namespace core {
    struct Point {
        float x{};
        float y{};
        int id{};
    };

    enum class StepKind {
        Start,
        PickPivot,
        ConsiderEdge,
        AddToHull,
        RemoveFromHull,
        Advance,
        Done
    };

    struct HullFrame {
        StepKind kind{StepKind::Start};
        std::vector<int> hull_indices;
        int active_a{-1};
        int active_b{-1};
        int active_c{-1};
        std::string label;
    };
}

#endif

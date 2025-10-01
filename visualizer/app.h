#ifndef VIZ_APP_H
#define VIZ_APP_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include "visualizer/renderer.h"
#include "algorithms/convex_hull_algorithm.h"
#include "core/types.h"
#include "generators/point_generator.h"

struct AlgoSpec {
    std::function<std::unique_ptr<ConvexHullAlgorithm>()> make;
};


struct GenSpec {
    std::function<std::unique_ptr<PointGenerator>()> make;
};

class App {
public:
    App(std::vector<AlgoSpec> algoSpecs, std::vector<GenSpec> genSpecs);
    void run();

private:
    std::vector<AlgoSpec> specs_;
    std::vector<std::string> algo_names_;
    int active_index_{0};
    std::unique_ptr<ConvexHullAlgorithm> active_;

    std::vector<GenSpec> gens_;
    std::vector<std::string> gen_names_;
    int active_gen_index_{0};

    Renderer renderer;
    std::vector<core::Point> points;
    double last_run_ms{0.0};
    bool have_data{false};

    static constexpr int WIN_W = 1024;
    static constexpr int WIN_H = 768;

    double step_accum_ms_{0.0};
    float  speed_multiplier_{1.0f};
    bool   slomo_on_{false};
    std::chrono::steady_clock::time_point tick_prev_{};
    static constexpr double BASE_INTERVAL_MS = 60.0;

    void regen_flow();
    void select_algo(int index, bool stop_play);
    void select_gen(int index);
    void restart_active(bool stop_play, bool recreate_instance);
};

#endif

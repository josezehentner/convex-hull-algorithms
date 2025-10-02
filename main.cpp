#include "visualizer/app.h"
#include "algorithms/quickhull.h"
#include "algorithms/andrew_algorithm.h"
#include "core/stopwatch.h"
#include "generators/circle_generator.h"
#include "generators/line_generator.h"
#include "generators/random_generator.h"
#include "generators/square_generator.h"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::vector<AlgoSpec> algoSpecs;
    algoSpecs.emplace_back([] { return std::make_unique<Quickhull>(); });
    algoSpecs.emplace_back([] { return std::make_unique<AndrewAlgorithm>(); });

    std::vector<GenSpec> genSpecs;
    genSpecs.push_back(GenSpec{ [] { return std::make_unique<RandomGenerator>(); } });
    genSpecs.push_back(GenSpec{ [] { return std::make_unique<CircleGenerator>(); } });
    genSpecs.push_back(GenSpec{ [] { return std::make_unique<SquareGenerator>(); } });
    genSpecs.push_back(GenSpec{ [] { return std::make_unique<LineGenerator>(); } });

    std::cout << "choose mode\n";
    std::cout << "1 visual player\n";
    std::cout << "2 performance only\n";
    int mode = 1;
    std::cin >> mode;
    std::cin.ignore();

    if (mode == 2) {
        Stopwatch sw;
        sw.reset();

        for (const GenSpec& genSpec : genSpecs) {
            std::unique_ptr<PointGenerator> points = genSpec.make();

            std::cout << "\nRunning with <" << points->name() << "> point placement:" << std::endl;

            for (const AlgoSpec& spec : algoSpecs) {
                std::unique_ptr<ConvexHullAlgorithm> algo = spec.make();
                algo->reset(points->generate(100000, 2000.0f, 1200.0f));

                sw.start();
                std::vector<int> hull = algo->run_full();
                sw.stop();
                long long ns = sw.ns();

                algo->report(ns, hull.size());
            }
        }

        return 0;
    }

    App app(std::move(algoSpecs), std::move(genSpecs));
    app.run();
    return 0;
}

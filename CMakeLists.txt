cmake_minimum_required(VERSION 4.0)
project(convex_hull)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(convex_hull
        main.cpp
        algorithms/convex_hull_algorithm.cpp
        algorithms/convex_hull_algorithm.h
        core/stopwatch.cpp
        core/stopwatch.h
        core/types.cpp
        core/types.h
        visualizer/renderer.cpp
        visualizer/renderer.h
        visualizer/app.cpp
        visualizer/app.h
        generators/point_generator.cpp
        generators/point_generator.h
        generators/random_generator.cpp
        generators/random_generator.h
        generators/circle_generator.cpp
        generators/circle_generator.h
        generators/square_generator.cpp
        generators/square_generator.h
        generators/line_generator.cpp
        generators/line_generator.h
        algorithms/andrew_algorithm.cpp
        algorithms/andrew_algorithm.h
        algorithms/quickhull.cpp
        algorithms/quickhull.h
)

target_include_directories(convex_hull PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(convex_hull PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)

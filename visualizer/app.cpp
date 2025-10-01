#include "visualizer/app.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>

using SteadyClock = std::chrono::steady_clock;

App::App(std::vector<AlgoSpec> algoSpecs, std::vector<GenSpec> genSpecs)
    : specs_(std::move(algoSpecs)),
      gens_(std::move(genSpecs)) {
    if (specs_.empty()) throw std::runtime_error("no algorithms configured");
    if (gens_.empty())  throw std::runtime_error("no generators configured");

    algo_names_.reserve(specs_.size());
    for (const auto& s : specs_) {
        auto tmp = s.make();
        algo_names_.push_back(tmp->name());
    }

    gen_names_.reserve(gens_.size());
    for (const auto& g : gens_) {
        auto tmp = g.make();
        gen_names_.push_back(tmp->name());
    }

    active_index_ = 0;
    active_gen_index_ = 0;
    active_ = specs_[active_index_].make();
}

void App::regen_flow() {
    auto gen = gens_[active_gen_index_].make();
    points = gen->generate(600,
                           static_cast<float>(WIN_W),
                           static_cast<float>(WIN_H));
    have_data = !points.empty();
}

void App::select_algo(int index, bool stop_play) {
    if (index < 0 || index >= static_cast<int>(specs_.size())) return;
    active_index_ = index;
    restart_active(stop_play, true);
}

void App::select_gen(int index) {
    if (index < 0 || index >= static_cast<int>(gens_.size())) return;
    active_gen_index_ = index;
    regen_flow();
    restart_active(true, true);
}

void App::restart_active(bool stop_play, bool recreate_instance) {
    if (stop_play) renderer.set_play(false);
    if (!have_data) return;
    if (recreate_instance) {
        active_ = specs_[active_index_].make();
    }
    active_->reset(points);
    active_->begin_stepping();
    last_run_ms = 0.0;
    step_accum_ms_ = 0.0;
}

void App::run() {
    renderer.open(WIN_W, WIN_H, "Convex Hull Visual");

    regen_flow();
    restart_active(true, false);

    tick_prev_ = SteadyClock::now();

    while (renderer.is_open()) {
        renderer.poll();
        if (!renderer.is_open()) break;

        auto now = SteadyClock::now();
        double dt_ms = std::chrono::duration<double, std::milli>(now - tick_prev_).count();
        tick_prev_ = now;

        if (renderer.wants_faster()) {
            speed_multiplier_ = std::min(8.0f, speed_multiplier_ * 1.25f);
            slomo_on_ = speed_multiplier_ < 1.0f;
        }
        if (renderer.wants_slower()) {
            speed_multiplier_ = std::max(0.1f, speed_multiplier_ / 1.25f);
            slomo_on_ = speed_multiplier_ < 1.0f;
        }
        if (renderer.wants_toggle_slomo()) {
            if (slomo_on_) {
                slomo_on_ = false;
                if (speed_multiplier_ < 1.0f) speed_multiplier_ = 1.0f;
            } else {
                slomo_on_ = true;
                speed_multiplier_ = 0.25f;
            }
        }

        if (int sel = renderer.wants_select_algo(); sel > 0) {
            select_algo(sel - 1, true);
            continue;
        }

        if (int gsel = renderer.wants_select_gen(); gsel > 0) {
            select_gen(gsel - 1);
            continue;
        }

        if (renderer.wants_reset()) {
            restart_active(true, true);
            continue;
        }

        if (renderer.wants_regen()) {
            regen_flow();
            restart_active(true, true);
            continue;
        }

        if (renderer.wants_step()) {
            if (!active_->step()) {
                renderer.set_play(false);
            }
        }

        if (renderer.is_playing()) {
            step_accum_ms_ += dt_ms;
            const double interval = BASE_INTERVAL_MS / std::max(0.01f, speed_multiplier_);
            int safety = 0;
            while (step_accum_ms_ >= interval && safety < 20) {
                if (!active_->step()) {
                    renderer.set_play(false);
                    step_accum_ms_ = 0.0;
                    break;
                }
                step_accum_ms_ -= interval;
                ++safety;
            }
        }

        renderer.draw_scene(points,
                            algo_names_,
                            active_index_,
                            gen_names_,
                            active_gen_index_,
                            active_->frame(),
                            last_run_ms,
                            speed_multiplier_,
                            slomo_on_);
    }
}

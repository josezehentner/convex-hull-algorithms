#ifndef VIZ_RENDERER_H
#define VIZ_RENDERER_H

#include <memory>
#include <string>
#include <vector>
#include "core/types.h"

namespace sf { class RenderWindow; }

class Renderer {
public:
    Renderer();
    ~Renderer();

    void open(int w, int h, const std::string& title);
    bool is_open() const;
    void poll();

    void draw_scene(const std::vector<core::Point>& pts,
                    const std::vector<std::string>& algo_labels,
                    int active_algo_index,
                    const std::vector<std::string>& gen_labels,
                    int active_gen_index,
                    const core::HullFrame& fr,
                    double last_run_ms,
                    float speed_multiplier,
                    bool slomo_on);

    bool is_playing() const;
    bool wants_step() const;
    bool wants_regen() const;
    int  wants_select_algo() const;
    int  wants_select_gen() const;
    bool wants_reset() const;

    bool wants_faster() const;
    bool wants_slower() const;
    bool wants_toggle_slomo() const;

    void set_play(bool on);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif

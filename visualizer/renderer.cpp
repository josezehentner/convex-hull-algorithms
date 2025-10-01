#include "visualizer/renderer.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <filesystem>
#include <sstream>
#include <iomanip>

struct Renderer::Impl {
    std::unique_ptr<sf::RenderWindow> win;
    bool play{false};
    bool step_now{false};
    bool regen{false};
    int  select_algo{0};
    int  select_gen{0};
    bool reset_now{false};

    bool faster_now{false};
    bool slower_now{false};
    bool toggle_slomo_now{false};

    bool overlay_loaded{false};
    bool overlay_ok{false};
    sf::Font overlay_font;
};

Renderer::Renderer() : impl(new Impl) {}
Renderer::~Renderer() {}

void Renderer::open(int w, int h, const std::string& title) {
    sf::Vector2u size{static_cast<unsigned>(w), static_cast<unsigned>(h)};
    impl->win.reset(new sf::RenderWindow(sf::VideoMode(size), title));
    impl->win->setFramerateLimit(60);
}

bool Renderer::is_open() const {
    return impl->win && impl->win->isOpen();
}

void Renderer::poll() {
    impl->step_now = false;
    impl->regen = false;
    impl->select_algo = 0;
    impl->select_gen = 0;
    impl->reset_now = false;
    impl->faster_now = false;
    impl->slower_now = false;
    impl->toggle_slomo_now = false;

    while (const std::optional<sf::Event> ev = impl->win->pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            impl->win->close();
            continue;
        }
        if (const auto* key = ev->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Enter: impl->step_now = true; break;
                case sf::Keyboard::Key::Space: impl->play = !impl->play; break;
                case sf::Keyboard::Key::R:     impl->regen = true; break;

                case sf::Keyboard::Key::Num1:  impl->select_algo = 1; break;
                case sf::Keyboard::Key::Num2:  impl->select_algo = 2; break;
                case sf::Keyboard::Key::Num3:  impl->select_algo = 3; break;
                case sf::Keyboard::Key::Num4:  impl->select_algo = 4; break;
                case sf::Keyboard::Key::Num5:  impl->select_algo = 5; break;
                case sf::Keyboard::Key::Num6:  impl->select_algo = 6; break;
                case sf::Keyboard::Key::Num7:  impl->select_algo = 7; break;
                case sf::Keyboard::Key::Num8:  impl->select_algo = 8; break;
                case sf::Keyboard::Key::Num9:  impl->select_algo = 9; break;

                case sf::Keyboard::Key::F1: impl->select_gen = 1; break;
                case sf::Keyboard::Key::F2: impl->select_gen = 2; break;
                case sf::Keyboard::Key::F3: impl->select_gen = 3; break;
                case sf::Keyboard::Key::F4: impl->select_gen = 4; break;
                case sf::Keyboard::Key::F5: impl->select_gen = 5; break;
                case sf::Keyboard::Key::F6: impl->select_gen = 6; break;
                case sf::Keyboard::Key::F7: impl->select_gen = 7; break;
                case sf::Keyboard::Key::F8: impl->select_gen = 8; break;
                case sf::Keyboard::Key::F9: impl->select_gen = 9; break;

                case sf::Keyboard::Key::Up:    impl->faster_now = true; break;
                case sf::Keyboard::Key::Down:  impl->slower_now = true; break;
                case sf::Keyboard::Key::S:     impl->toggle_slomo_now = true; break;
                case sf::Keyboard::Key::Escape: impl->reset_now = true; break;
                default: break;
            }
        }
    }
}

bool Renderer::wants_step() const { return impl->step_now; }
bool Renderer::wants_regen() const { return impl->regen; }
int  Renderer::wants_select_algo() const { return impl->select_algo; }
int  Renderer::wants_select_gen() const { return impl->select_gen; }
bool Renderer::wants_reset() const { return impl->reset_now; }
bool Renderer::is_playing() const { return impl->play; }
bool Renderer::wants_faster() const { return impl->faster_now; }
bool Renderer::wants_slower() const { return impl->slower_now; }
bool Renderer::wants_toggle_slomo() const { return impl->toggle_slomo_now; }
void Renderer::set_play(bool on) { impl->play = on; }

static sf::Color color_point() { return sf::Color(180, 180, 180); }
static sf::Color color_hull()  { return sf::Color(0, 200, 255); }
static sf::Color color_a()     { return sf::Color(255, 80, 80); }
static sf::Color color_b()     { return sf::Color(80, 255, 120); }
static sf::Color color_c()     { return sf::Color(255, 220, 80); }

void Renderer::draw_scene(const std::vector<core::Point>& pts,
                          const std::vector<std::string>& algo_labels,
                          int active_algo_index,
                          const std::vector<std::string>& gen_labels,
                          int active_gen_index,
                          const core::HullFrame& fr,
                          double last_run_ms,
                          float speed_multiplier,
                          bool slomo_on) {
    if (!impl->win || !impl->win->isOpen()) return;

    impl->win->clear();

    for (auto& p : pts) {
        sf::CircleShape dot(3.0f);
        dot.setPosition({p.x - 3.0f, p.y - 3.0f});
        dot.setFillColor(color_point());
        impl->win->draw(dot);
    }

    if (fr.hull_indices.size() >= 2) {
        for (std::size_t i = 0; i < fr.hull_indices.size(); ++i) {
            int a = fr.hull_indices[i];
            int b = fr.hull_indices[(i + 1) % fr.hull_indices.size()];
            sf::Vertex seg[] = {
                sf::Vertex{sf::Vector2f(pts[a].x, pts[a].y), color_hull()},
                sf::Vertex{sf::Vector2f(pts[b].x, pts[b].y), color_hull()}
            };
            impl->win->draw(seg, 2, sf::PrimitiveType::Lines);
        }
    }

    auto draw_mark = [&](int idx, sf::Color c) {
        if (idx < 0 || idx >= static_cast<int>(pts.size())) return;
        sf::CircleShape mark(6.0f);
        mark.setPosition({pts[idx].x - 6.0f, pts[idx].y - 6.0f});
        mark.setFillColor(c);
        impl->win->draw(mark);
    };
    draw_mark(fr.active_a, color_a());
    draw_mark(fr.active_b, color_b());
    draw_mark(fr.active_c, color_c());

    if (!impl->overlay_loaded) {
        if (!impl->overlay_ok) impl->overlay_ok = impl->overlay_font.openFromFile("../assets/DejaVuSans.ttf");
        impl->overlay_loaded = true;
    }

    if (impl->overlay_ok) {
        const unsigned kSize = 14;
        float margin = 8.0f;

        sf::Text algoPrefix(impl->overlay_font, "Algorithms: ", kSize);
        algoPrefix.setFillColor(sf::Color::White);

        std::vector<sf::Text> algoItems;
        algoItems.reserve(algo_labels.size());
        for (std::size_t i = 0; i < algo_labels.size(); ++i) {
            std::string label = std::to_string(static_cast<int>(i + 1)) + " " + algo_labels[i];
            if (static_cast<int>(i) == active_algo_index) label = "[" + label + "]";
            sf::Text t(impl->overlay_font, label + "    ", kSize);
            t.setFillColor(sf::Color::White);
            if (static_cast<int>(i) == active_algo_index) t.setStyle(sf::Text::Style::Bold);
            algoItems.push_back(t);
        }

        sf::Text genPrefix(impl->overlay_font, "Generation: ", kSize);
        genPrefix.setFillColor(sf::Color::White);

        std::vector<sf::Text> genItems;
        genItems.reserve(gen_labels.size());
        for (std::size_t i = 0; i < gen_labels.size(); ++i) {
            std::string label = "F" + std::to_string(static_cast<int>(i + 1)) + " " + gen_labels[i];
            if (static_cast<int>(i) == active_gen_index) label = "[" + label + "]";
            sf::Text t(impl->overlay_font, label + "    ", kSize);
            t.setFillColor(sf::Color::White);
            if (static_cast<int>(i) == active_gen_index) t.setStyle(sf::Text::Style::Bold);
            genItems.push_back(t);
        }

        std::ostringstream ss;
        ss << "Controls: Enter next, Space play, Esc reset, R random, numbers choose algo, F1 to F9 choose generator, Up faster, Down slower, S slomo";
        sf::Text controlsText(impl->overlay_font, ss.str(), kSize);
        controlsText.setFillColor(sf::Color::White);

        std::ostringstream spd;
        spd << "Speed " << std::fixed << std::setprecision(2) << speed_multiplier << "x";
        if (slomo_on) spd << "  slomo on";
        if (last_run_ms > 0.0) spd << "    last run ms " << static_cast<int>(last_run_ms);
        sf::Text speedText(impl->overlay_font, spd.str(), kSize);
        speedText.setFillColor(sf::Color::White);

        sf::Text status(impl->overlay_font, fr.label, kSize);
        status.setFillColor(sf::Color::White);

        float h1 = std::max(algoPrefix.getLocalBounds().size.y, static_cast<float>(kSize));
        for (auto& t : algoItems) h1 = std::max(h1, t.getLocalBounds().size.y);

        float hG = std::max(genPrefix.getLocalBounds().size.y, static_cast<float>(kSize));
        for (auto& t : genItems) hG = std::max(hG, t.getLocalBounds().size.y);

        float h2 = controlsText.getLocalBounds().size.y;
        float h3 = speedText.getLocalBounds().size.y;
        float h4 = status.getLocalBounds().size.y;

        float total_h = h1 + hG + h2 + h3 + h4 + 24.0f;

        auto winSize = impl->win->getSize();
        float y = static_cast<float>(winSize.y) - total_h - margin;

        sf::RectangleShape bg;
        bg.setPosition({0.f, y - 6.0f});
        bg.setSize({static_cast<float>(winSize.x), total_h + 12.0f});
        bg.setFillColor(sf::Color(0, 0, 0, 140));
        impl->win->draw(bg);

        float x = margin;
        algoPrefix.setPosition({x, y});
        impl->win->draw(algoPrefix);
        x += algoPrefix.getLocalBounds().size.x;
        for (auto& t : algoItems) {
            t.setPosition({x, y});
            impl->win->draw(t);
            x += t.getLocalBounds().size.x;
        }

        float yG = y + h1 + 2.0f;
        x = margin;
        genPrefix.setPosition({x, yG});
        impl->win->draw(genPrefix);
        x += genPrefix.getLocalBounds().size.x;
        for (auto& t : genItems) {
            t.setPosition({x, yG});
            impl->win->draw(t);
            x += t.getLocalBounds().size.x;
        }

        float y2 = yG + hG + 2.0f;
        controlsText.setPosition({margin, y2});
        impl->win->draw(controlsText);

        float y3 = y2 + h2 + 2.0f;
        speedText.setPosition({margin, y3});
        impl->win->draw(speedText);

        float y4 = y3 + h3 + 2.0f;
        status.setPosition({margin, y4});
        impl->win->draw(status);
    }

    impl->win->display();
}

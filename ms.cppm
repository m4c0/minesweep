export module ms;
import :atlas;
import casein;
import quack;

using namespace ms;

constexpr const auto grid_size = 36;
constexpr const auto max_bombs = grid_size * 8;

class game_grid : public quack::grid_renderer<grid_size, grid_size, cell> {
  unsigned m_width{1};
  unsigned m_height{1};
  unsigned m_ticks{};

  void render() {
    fill_uv(uv_filler{});
    fill_colour([](const auto &b) {
      if (!b.visible)
        return quack::colour{};
      if (b.bomb) {
        return quack::colour{0.3, 0, 0, 1};
      } else {
        float f = b.count / 8.0f;
        return quack::colour{0, f, 0, 1};
      }
    });
  }

  void setup_bombs() {
    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = (m_ticks * 115249 ^ m_ticks * 331319) % cells;
      while (at(p).bomb) {
        p = (++p * 60493) % cells;
      }
      at(p).bomb = true;
    }
  }

  void update_numbers() {
    for (auto y = 0; y < grid_size; y++) {
      for (auto x = 0; x < grid_size; x++) {
        update_numbers_at(x, y);
      }
    }
  }

  void update_numbers_at(unsigned x, unsigned y) {
    if (at(x, y).bomb)
      return;

    for (auto dy = -1; dy <= 1; dy++) {
      const auto ny = y + dy;
      if ((ny < 0) || (ny >= grid_size))
        continue;
      for (auto dx = -1; dx <= 1; dx++) {
        const auto nx = x + dx;
        if ((nx < 0) || (nx >= grid_size))
          continue;
        if (at(nx, ny).bomb)
          at(x, y).count++;
      }
    }
  }

  void build_atlas() { load_atlas(atlas::width, atlas::height, atlas{}); }

public:
  void click() {
    current_hover().consume([this](auto idx) {
      at(idx).visible = true;
      render();
    });
  }

  void reset_level() {
    reset_grid();
    setup_bombs();
    update_numbers();
    build_atlas();
    render();
  }

  void repaint() {
    m_ticks++;
    grid_renderer::repaint();
  }

  void resize(int w, int h) {
    m_width = w;
    m_height = h;
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};
  r.process_event(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.reset_level();
    break;
  case casein::RESIZE_WINDOW: {
    const auto &[w, h, scale, live] = *e.as<casein::events::resize_window>();
    r.resize(w, h);
    break;
  }
  case casein::KEY_DOWN:
    switch (*e.as<casein::events::key_down>()) {
    case casein::K_SPACE:
      r.reset_level();
      break;
    default:
      break;
    }
    break;
  case casein::MOUSE_DOWN:
    r.click();
    break;
  default:
    break;
  }
}

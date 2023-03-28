export module ms;
import casein;
import quack;

enum cell { empty, bomb };

constexpr const auto max_bombs = 36;
constexpr const auto grid_size = 36;

class game_grid : public quack::grid_renderer<grid_size, grid_size, cell> {
  unsigned m_ticks{};

public:
  void reset_level() {
    reset_grid();
    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = (m_ticks * 115249 ^ m_ticks * 331319) % cells;
      while (at(p) != empty) {
        p = (++p * 60493) % cells;
      }
      at(p) = bomb;
    }

    constexpr const auto n = 16;
    load_atlas(n, n, [](quack::u8_rgba *img) {
      for (auto i = 0; i < n; i++) {
        img[i] = img[n * i] = {64, 64, 64, 255};
      }
    });
    fill_uv([](auto b) { return quack::uv{{0, 0}, {1, 1}}; });
    fill_colour([](auto b) {
      switch (b) {
      case bomb:
        return quack::colour{0.3, 0, 0, 1};
      default:
        return quack::colour{};
      }
    });
  }

  void repaint() {
    m_ticks++;
    grid_renderer::repaint();
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.reset_level();
    break;
  case casein::REPAINT:
    r.repaint();
    break;
  case casein::KEY_DOWN:
    switch (e.as<casein::events::key_down>().key()) {
    case casein::K_SPACE:
      r.reset_level();
      break;
    default:
      break;
    }
    break;
  case casein::QUIT:
    r.quit();
    break;
  default:
    break;
  }
}

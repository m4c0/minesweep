export module ms;
import :atlas;
import casein;
import quack;

using namespace ms;

constexpr const auto grid_size = 36;
constexpr const auto max_bombs = grid_size * 4;

class game_grid {
  quack::renderer m_r{1};
  quack::grid_ilayout<grid_size, grid_size, cell> m_grid{&m_r};
  unsigned m_ticks{};

  static constexpr const auto cells = decltype(m_grid)::cells;

  void render() {
    m_grid.fill_uv(uv_filler{});
    m_grid.fill_colour([](const auto &b) {
      if (!b.visible)
        return quack::colour{0, 0, 0, 1};
      if (b.bomb) {
        return quack::colour{0.3, 0, 0, 1};
      } else {
        float f = b.count / 8.0f;
        return quack::colour{0, f * 0.3f, 0, 1};
      }
    });
  }

  void setup_bombs() {
    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = (m_ticks * 115249 ^ m_ticks * 331319) % cells;
      while (m_grid.at(p).bomb) {
        p = (++p * 60493) % cells;
      }
      m_grid.at(p).bomb = true;
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
    if (m_grid.at(x, y).bomb)
      return;

    for (auto dy = -1; dy <= 1; dy++) {
      const auto ny = y + dy;
      if ((ny < 0) || (ny >= grid_size))
        continue;
      for (auto dx = -1; dx <= 1; dx++) {
        const auto nx = x + dx;
        if ((nx < 0) || (nx >= grid_size))
          continue;
        if (m_grid.at(nx, ny).bomb)
          m_grid.at(x, y).count++;
      }
    }
  }

  void build_atlas() { m_r.load_atlas(atlas::width, atlas::height, atlas{}); }

  void fill(unsigned x, unsigned y) {
    auto &p = m_grid.at(x, y);
    if (p.visible)
      return;

    p.visible = true;

    if (p.count != 0 || p.bomb)
      return;

    for (auto dx = -1; dx <= 1; dx++) {
      auto nx = x + dx;
      if (nx < 0 || nx >= grid_size)
        continue;
      for (auto dy = -1; dy <= 1; dy++) {
        auto ny = y + dy;
        if (ny < 0 || ny >= grid_size)
          continue;
        fill(nx, ny);
      }
    }
  }

public:
  void click() {
    m_grid.current_hover().consume([this](auto idx) {
      fill(idx % grid_size, idx / grid_size);
      render();
    });
  }

  void process_event(const casein::event &e) {
    m_r.process_event(e);
    m_grid.process_event(e);
  }

  void reset_level() {
    m_grid.reset_grid();
    setup_bombs();
    update_numbers();
    build_atlas();
    render();
  }

  void repaint() { m_ticks++; }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid gg{};
  gg.process_event(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    gg.reset_level();
    break;
  case casein::KEY_DOWN:
    switch (*e.as<casein::events::key_down>()) {
    case casein::K_SPACE:
      gg.reset_level();
      break;
    default:
      break;
    }
    break;
  case casein::MOUSE_DOWN: {
    const auto &md = *e.as<casein::events::mouse_down>();
    switch (md.button) {
    case 0:
      gg.click();
      break;
    };
    break;
  }
  case casein::GESTURE:
    switch (*e.as<casein::events::gesture>()) {
    case casein::G_TAP_1:
      gg.click();
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

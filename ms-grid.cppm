export module ms:grid;
import :atlas;
import rng;

export namespace ms {
constexpr const auto grid_size = 36;
constexpr const auto max_bombs = grid_size * 4;
constexpr const auto cells = grid_size * grid_size;

struct rgba {
  float r, g, b, a;
};
struct inst {
  float pos[2];
  uv uv;
  rgba bg;
};
static_assert(sizeof(inst) == 10 * sizeof(float));

constexpr const auto instance_size = sizeof(inst);
constexpr const auto instance_buf_size = cells * instance_size;

class grid {
  cell m_cells[cells]{};

  void setup_bombs() {
    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = rng::rand(cells);
      while (m_cells[p].bomb) {
        p = rng::rand(cells);
      }
      m_cells[p].bomb = true;
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

  void fill(unsigned x, unsigned y) {
    auto &p = at(x, y);
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
  grid() : m_cells{} {
    rng::seed();
    setup_bombs();
    update_numbers();
  }

  [[nodiscard]] constexpr cell &at(unsigned x, unsigned y) {
    return m_cells[y * grid_size + x];
  }

  void click(int x, int y) {
    if (x >= 0 && y >= 0 && x < grid_size && y < grid_size) {
      fill(x, y);
    }
  }

  void flag(int x, int y) {
    if (x >= 0 && y >= 0 && x < grid_size && y < grid_size) {
      auto &g = at(x, y);
      g.flagged = !g.flagged;
    }
  }

  void load(inst *buf) const {
    for (auto i = 0; i < cells; i++) {
      auto &b = buf[i];
      const auto &c = m_cells[i];

      b.pos[0] = i % grid_size;
      b.pos[1] = i / grid_size;

      auto uv = uv_filler::uv(c);
      b.uv = uv;

      if (!c.visible) {
        b.bg = {0, 0, 0, 1};
      } else if (c.bomb) {
        b.bg = {0.3, 0, 0, 1};
      } else {
        float f = c.count / 8.0f;
        b.bg = {0, f * 0.3f, 0, 1};
      }
    }
  }
};
} // namespace ms

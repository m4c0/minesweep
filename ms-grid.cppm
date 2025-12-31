export module ms:grid;
import :atlas;
import dotz;
import rng;

export namespace ms {
constexpr const auto grid_size = 36;
constexpr const auto max_bombs = grid_size * 4;
constexpr const auto cells = grid_size * grid_size;

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

  [[nodiscard]] static constexpr dotz::vec4 colour_of(cell cell) {
    if (!cell.visible) {
      return {0, 0, 0, 1};
    } else if (cell.bomb) {
      return {0.3, 0, 0, 1};
    } else {
      float f = cell.count / 8.0f;
      return {0, f * 0.3f, 0, 1};
    }
  }

public:
  grid() : m_cells{} {
    rng::seed();
    setup_bombs();
    update_numbers();
  }

  [[nodiscard]] constexpr auto bomb_count() {
    int res = max_bombs;
    for (auto &c : m_cells) {
      if (c.flagged)
        res--;
    }
    return res;
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

  void load(auto & m) const {
    int bombs = 0;
    for (auto i = 0; i < cells; i++) {
      const auto &cell = m_cells[i];
      const float x = i % grid_size;
      const float y = i / grid_size;

      m->push({
        .pos { x, y },
        .colour = colour_of(cell),
        .uv = uv(cell),
      });
      if (cell.bomb) bombs++;
      if (cell.flagged) bombs--;
    }
    if (bombs < 0) bombs = 0;

    for (unsigned i = 0; i < 4; i++) {
      m->push({
        .pos { i, -2 },
        .uv = s_label + i,
      });
    }
    if (bombs == 0) {
      m->push({
        .pos { 4, -2 },
        .uv = s_label_0,
      });
    } else {
      unsigned n = 0;
      unsigned acc = bombs;
      while (acc) {
        acc /= 10;
        n++;
      }
      for (auto i = 0; i < n; i++) {
        m->push({
          .pos { 4 + n - i, -2 },
          .uv = uv_label(bombs % 10),
        });
        bombs /= 10;
      }
    }
  }
};
} // namespace ms

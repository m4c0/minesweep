export module ms:grid;
import :atlas;
import dotz;
import file;
import hai;
import rng;
import silog;

export namespace ms {
  struct game_parameters {
    unsigned difficulty;
    float label_w;
    unsigned grid_size;
    unsigned max_bombs;
  };

class grid {
  game_parameters m_p;
  hai::array<cell> m_cells;

  void setup_bombs(unsigned max_bombs) {
    const auto cells = m_cells.size();

    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = rng::rand(cells);
      while (m_cells[p].bomb) {
        p = rng::rand(cells);
      }
      m_cells[p].bomb = true;
    }
  }

  void update_numbers() {
    for (auto y = 0; y < grid_size(); y++) {
      for (auto x = 0; x < grid_size(); x++) {
        update_numbers_at(x, y);
      }
    }
  }

  void update_numbers_at(unsigned x, unsigned y) {
    if (at(x, y).bomb) return;

    for (auto dy = -1; dy <= 1; dy++) {
      const auto ny = y + dy;
      if ((ny < 0) || (ny >= grid_size())) continue;
      for (auto dx = -1; dx <= 1; dx++) {
        const auto nx = x + dx;
        if ((nx < 0) || (nx >= grid_size())) continue;
        if (at(nx, ny).bomb) at(x, y).count++;
      }
    }
  }

  void fill(unsigned x, unsigned y) {
    auto &p = at(x, y);
    if (p.visible) return;

    p.flagged = false;
    p.visible = true;

    if (p.count != 0 || p.bomb) return;

    for (auto dx = -1; dx <= 1; dx++) {
      auto nx = x + dx;
      if (nx < 0 || nx >= grid_size()) continue;
      for (auto dy = -1; dy <= 1; dy++) {
        auto ny = y + dy;
        if (ny < 0 || ny >= grid_size()) continue;
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

  [[nodiscard]] constexpr cell &at(unsigned x, unsigned y) {
    return m_cells[y * grid_size() + x];
  }

public:
  grid(game_parameters p) :
    m_p { p }
  , m_cells { p.grid_size * p.grid_size }
  { reset(); }

  void reset() {
    rng::seed();
    for (auto & c : m_cells) c = {};
    setup_bombs(m_p.max_bombs);
    update_numbers();
  }

  constexpr unsigned grid_size() const { return m_p.grid_size; }

  bool can_hover(int x, int y) {
    if (x < 0 || y < 0 || x >= grid_size() || y >= m_p.grid_size) return false;
    return !at(x, y).visible;
  }

  enum class click_outcome { none, fill, bomb };
  click_outcome click(int x, int y) {
    using enum click_outcome;

    if (x >= 0 && y >= 0 && x < grid_size() && y < m_p.grid_size) {
      if (at(x, y).flagged) return none;

      this->fill(x, y);
      return at(x, y).bomb ? bomb : fill;
    }

    return none;
  }

  void flag(int x, int y) {
    if (x >= 0 && y >= 0 && x < grid_size() && y < m_p.grid_size) {
      auto &g = at(x, y);
      g.flagged = !g.flagged;
    }
  }

  void won(auto & m, float secs) const {
    static constexpr const auto grav = 18;
    static constexpr const auto v_0 = -20;

    for (auto i = 0; i < m_cells.size(); i++) {
      const auto &cell = m_cells[i];
      float x = i % grid_size();
      float y = i / grid_size();

      x += secs * (x - grid_size()/2.f);
      y += v_0 * (grid_size() - y)/grid_size() * secs + grav * secs * secs;

      m->push({
        .pos { x, y },
        .colour = colour_of(cell),
        .uv = uv(cell),
      });
    }
  }

  enum class draw_outcome { none, won }; 
  draw_outcome draw(auto & m) const {
    int visible = 0;
    int flagged = 0;
    int bombs = 0;
    for (auto i = 0; i < m_cells.size(); i++) {
      const auto &cell = m_cells[i];
      const float x = i % grid_size();
      const float y = i / grid_size();

      m->push({
        .pos { x, y },
        .colour = colour_of(cell),
        .uv = uv(cell),
      });
      if (cell.bomb) bombs++;
      if (cell.flagged) flagged++;
      if (cell.visible) visible++;
    }

    constexpr const auto label_y = -1.5f;
    for (unsigned i = 0; i < 4; i++) {
      m->push({
        .pos { i, label_y },
        .uv = s_label + i,
      });
    }
    if (bombs <= flagged) {
      m->push({
        .pos { 4, label_y },
        .uv = s_label_0,
      });
    } else {
      unsigned n = 0;
      unsigned b = bombs - flagged;
      unsigned acc = b;
      while (acc) {
        acc /= 10;
        n++;
      }
      for (auto i = 0; i < n; i++) {
        m->push({
          .pos { 3.2f + (n - i) * 0.8f, label_y },
          .uv = uv_label(b % 10),
        });
        b /= 10;
      }
    }

    for (auto i = 0U; i < 5; i++) {
      m->push({
        .pos { grid_size() + i - m_p.label_w, label_y },
        .uv = m_p.difficulty + i,
      });
    }

    return visible + flagged == m_cells.size() && bombs == flagged 
      ? draw_outcome::won
      : draw_outcome::none;;
  }

  void load(unsigned id) try {
    file::reader f { id };
    if (!f) return;

    if (f.read<unsigned>() != 'M4MS') throw "invalid file type";
    if (f.read<unsigned>() != 1) throw "invalid file version";

    for (auto & c : m_cells) f.read(&c);

    if (f.read<unsigned>() != 'M4ME') throw "invalid end-of-file marker";
  } catch (file::error) {
    silog::errorf("could not read data from save id=%d", id);
    reset();
  } catch (const char * msg) {
    silog::errorf("error loading save id=%d: %s", id, msg);
    reset();
  }

  void save(unsigned id) try {
    file::writer f { id };
    if (!f) return;

    f.write<unsigned>('M4MS');
    f.write<unsigned>(1);

    for (auto & c : m_cells) f.write(c);

    f.write<unsigned>('M4ME');
    silog::infof("stored save file with id=%d", id);
  } catch (file::error) {
    silog::errorf("could not read data from save id=%d", id);
  } catch (const char * msg) {
    silog::errorf("error loading save id=%d: %s", id, msg);
  }
};
} // namespace ms

export module ms:upc;
import :grid;
import casein;
import dotz;
import quack;

namespace ms {
class upc {
  dotz::vec2 m_area_pos{};
  dotz::vec2 m_area_sz{};
  dotz::vec2 m_sel{-1, -1};

public:
  upc() = default;

  void update() {
    auto grid_size = ms::grid_size;
    auto mouse = quack::mouse_tracker::instance().mouse_pos_rel();

    const auto m = grid_size * 0.1;
    auto asp = quack::extent_tracker::instance().screen_aspect();
    auto aw = asp > 1 ? asp : 1;
    auto ah = asp > 1 ? 1 : asp;

    m_area_sz = dotz::vec2{grid_size} + m * 2;
    m_area_sz.x *= aw;
    m_area_sz.y /= ah;
    m_area_pos = (m_area_sz - grid_size) / 2;

    m_sel = dotz::floor(-m_area_pos + m_area_sz * mouse);
  }

  [[nodiscard]] constexpr auto sel() const noexcept { return m_sel; }
};
static_assert(sizeof(upc) == 6 * sizeof(float));

class pc_handler : public casein::handler {
  ms::upc m_pc{};

  pc_handler() = default;

public:
  [[nodiscard]] constexpr const auto *operator*() const noexcept {
    return &m_pc;
  }

  void mouse_move(const casein::events::mouse_move &e) override {
    m_pc.update();
  }
  void resize_window(const casein::events::resize_window &e) override {
    m_pc.update();
  }

  static auto &instance() {
    static pc_handler i{};
    return i;
  }
  static auto *pc() { return *instance(); }
};
} // namespace ms

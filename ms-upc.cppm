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
    dotz::vec2 mouse{casein::mouse_pos.x, casein::mouse_pos.y};

    const auto m = grid_size * 0.1;
    float scr_w = casein::window_size.x;
    float scr_h = casein::window_size.y;
    auto asp = scr_w / scr_h;
    auto aw = asp > 1 ? asp : 1;
    auto ah = asp > 1 ? 1 : asp;

    m_area_sz = dotz::vec2{grid_size} + m * 2;
    m_area_sz.x *= aw;
    m_area_sz.y /= ah;
    m_area_pos = (m_area_sz - grid_size) / 2;

    m_sel = dotz::floor(-m_area_pos + m_area_sz * mouse);
  }

  [[nodiscard]] constexpr auto sel() const noexcept { return m_sel; }
} pc;
static_assert(sizeof(upc) == 6 * sizeof(float));
} // namespace ms

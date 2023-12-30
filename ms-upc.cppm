export module ms:upc;
import dotz;

namespace ms {
export class upc {
  dotz::vec2 m_area_pos{};
  dotz::vec2 m_area_sz{};
  dotz::vec2 m_sel{-1, -1};

public:
  upc() = default;

  void update(int grid_size, dotz::vec2 mouse, dotz::vec2 scr_size) {
    const auto m = grid_size * 0.1;
    auto asp = scr_size.x / scr_size.y;
    auto aw = asp > 1 ? asp : 1;
    auto ah = asp > 1 ? 1 : asp;

    m_area_sz = dotz::vec2{grid_size} + m * 2;
    m_area_sz.x *= aw;
    m_area_sz.y /= ah;
    m_area_pos = (m_area_sz - grid_size) / 2;

    m_sel = dotz::floor(-m_area_pos + m_area_sz * mouse / scr_size);
  }

  [[nodiscard]] constexpr auto sel() const noexcept { return m_sel; }
};
static_assert(sizeof(upc) == 6 * sizeof(float));
} // namespace ms

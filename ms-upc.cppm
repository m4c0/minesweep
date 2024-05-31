export module ms:upc;
import :grid;
import casein;
import dotz;
import quack;

namespace ms {
struct upc {
  dotz::vec2 area_pos{};
  dotz::vec2 area_sz{};
  dotz::vec2 sel{-1, -1};
};
static_assert(sizeof(upc) == 6 * sizeof(float));

[[nodiscard]] upc calculate_upc() {
  auto grid_size = ms::grid_size;
  auto scr = casein::window_size;
  auto mouse = casein::mouse_pos / scr;

  const auto m = grid_size * 0.1;
  auto asp = scr.x / scr.y;
  auto aw = asp > 1 ? asp : 1;
  auto ah = asp > 1 ? 1 : asp;

  auto area_sz = dotz::vec2{grid_size} + m * 2;
  area_sz.x *= aw;
  area_sz.y /= ah;

  auto area_pos = (area_sz - grid_size) / 2;
  auto sel = dotz::floor(-area_pos + area_sz * mouse);

  return upc{
      .area_pos = area_pos,
      .area_sz = area_sz,
      .sel = sel,
  };
}
} // namespace ms

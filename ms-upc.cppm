export module ms:upc;
import casein;
import dotz;
import v;

namespace ms {
[[nodiscard]] v::upc calculate_upc(unsigned grid_size) {
  auto scr = casein::window_size;
  auto mouse = casein::mouse_pos / scr;

  auto asp = scr.x / scr.y;
  auto aw = asp > 1 ? asp : 1;
  auto ah = asp > 1 ? 1 : asp;

  auto area_sz = dotz::vec2 { grid_size } + 4;
  area_sz.x *= aw;
  area_sz.y /= ah;

  auto area_pos = (area_sz - grid_size) / 2;
  auto sel = dotz::floor(-area_pos + area_sz * mouse);

  return v::upc{
    .client_area { area_pos, area_sz },
    .hover = sel,
  };
}
} // namespace ms

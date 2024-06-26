#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"
#pragma leco add_shader "ms-label.vert"
#pragma leco add_shader "ms-label.frag"
#pragma leco add_resource "atlas.png"

export module ms:vulkan;
import :grid;
import :upc;
import dotz;
import quack;
import vee;
import voo;

static quack::donald::atlas_t *my_atlas(voo::device_and_queue *dq) {
  return new voo::sires_image{"atlas.png", dq};
}

static unsigned update_data(quack::mapped_buffers all) {
  return ms::grid::instance().load(all);
}

namespace ms {
void init_quack() {
  using namespace quack::donald;

  app_name("minesweep");
  max_quads(cells);

  auto cs = dotz::vec2{grid_size};
  push_constants({
      .grid_pos = cs * 0.5,
      .grid_size = cs * 1.2,
  });
  atlas(my_atlas);
  data(update_data);
}

void redraw() { quack::donald::data(update_data); }

} // namespace ms

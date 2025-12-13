#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"
// #pragma leco add_shader "ms-label.vert"
// #pragma leco add_shader "ms-label.frag"
#pragma leco add_resource "atlas.png"

export module ms:vulkan;
import :grid;
import :upc;
import dotz;
import v;

// static void my_atlas() {
//   return v::texture::load("atlas.png");
// }
// 
// static void update_data(v::mapper * m) {
//   ms::grid::instance().load(m);
// }

namespace ms {
  void init_quack() {
    //using namespace quack::donald;

    //app_name("minesweep");
    //max_quads(cells);

    //auto cs = dotz::vec2{grid_size};
    //push_constants({
    //    .grid_pos = cs * 0.5,
    //    .grid_size = cs * 1.2,
    //});
    //atlas(my_atlas);
    //data(update_data);
  }

  void redraw() { 
    //quack::donald::data(update_data);
  }
}

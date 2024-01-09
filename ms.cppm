#pragma leco app
#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module ms;
import :events;
import :grid;
import :upc;
import :vulkan;
import casein;
import dotz;
import hai;

using point = dotz::vec2;

class pc_handler : public casein::handler {
  point m_screen_size{};
  point m_mouse_pos{};
  ms::upc m_pc{};

public:
  [[nodiscard]] constexpr const auto *operator*() const noexcept {
    return &m_pc;
  }

  void mouse_move(const casein::events::mouse_move &e) override {
    m_mouse_pos.x = (*e).x;
    m_mouse_pos.y = (*e).y;
    m_pc.update(ms::grid_size, m_mouse_pos, m_screen_size);
  }
  void resize_window(const casein::events::resize_window &e) override {
    m_screen_size.x = (*e).width;
    m_screen_size.y = (*e).height;
    m_pc.update(ms::grid_size, m_mouse_pos, m_screen_size);
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static ms::vulkan t{};
  static ms::casein_handler ch{};
  static pc_handler pc{};

  pc.handle(e);
  t.set_pc(*pc);
  ch.set_pc(*pc);

  ch.handle(e);

  if (t.ready())
    if (ch.dirty()) {
      t.load(ch.cells());
    }
  t.handle(e);
}

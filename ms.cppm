#pragma leco app
#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module ms;
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

class casein_handler : public casein::handler {
  bool m_render{};
  ms::grid m_cells{};
  const ms::upc *m_pc{};

public:
  void set_pc(const ms::upc *m) { m_pc = m; }

  void render() { m_render = true; }

  void click() {
    auto [x, y] = m_pc->sel();
    m_cells.click(x, y);
    render();
  }

  void flag() {
    auto [x, y] = m_pc->sel();
    m_cells.flag(x, y);
    render();
  }

  void reset_level() {
    m_cells = {};
    render();
  }

public:
  [[nodiscard]] constexpr bool dirty() noexcept {
    auto r = m_render;
    m_render = false;
    return r;
  }
  [[nodiscard]] constexpr const auto *cells() noexcept { return &m_cells; }

  void create_window(const casein::events::create_window &e) override {
    reset_level();
  }
  void key_down(const casein::events::key_down &e) override {
    switch (*e) {
    case casein::K_SPACE:
      reset_level();
      break;
    default:
    }
  }
  void mouse_down(const casein::events::mouse_down &e) override {
    switch (*e) {
    case casein::M_LEFT:
      click();
      break;
    case casein::M_RIGHT:
      flag();
      break;
    }
  }
  void touch_down(const casein::events::touch_down &e) override {
    if ((*e).long_press)
      flag();
  }
  void gesture(const casein::events::gesture &e) override {
    if (*e == casein::G_TAP_1)
      click();
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static ms::vulkan t{};
  static casein_handler ch{};
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

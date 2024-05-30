export module ms:events;
import :grid;
import :label;
import :vulkan;
import :upc;
import casein;

namespace ms {
class casein_handler {
  ms::grid m_cells{};

  casein_handler() = default;

public:
  void render() {
    vulkan::instance().load(&m_cells);
    label::instance().update_bomb_count(m_cells.bomb_count());
  }

  void click() {
    auto [x, y] = pc_handler::pc()->sel();
    m_cells.click(x, y);
    render();
  }

  void flag() {
    auto [x, y] = pc_handler::pc()->sel();
    m_cells.flag(x, y);
    render();
  }

  void reset_level() {
    m_cells = {};
    render();
  }

public:
  /*
  void create_window() override {
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
  */

  static auto &instance() {
    static casein_handler i{};
    return i;
  }
};
} // namespace ms

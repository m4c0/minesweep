#pragma leco app
export module ms;
import :grid;
import :upc;
import :vulkan;
import casein;

static void click() {
  auto [x, y] = ms::calculate_upc().sel;
  ms::grid::instance().click(x, y);
  ms::redraw();
}

static void flag() {
  auto [x, y] = ms::calculate_upc().sel;
  ms::grid::instance().flag(x, y);
  ms::redraw();
}

static void reset_level() {
  ms::grid::instance() = {};
  ms::redraw();
}

struct init {
  init() {
    using namespace casein;
    set_title("Minesweep");
    handle(KEY_DOWN, K_SPACE, reset_level);
    handle(MOUSE_DOWN, M_LEFT, click);
    handle(MOUSE_DOWN, M_RIGHT, flag);
    handle(GESTURE, G_TAP_1, click);
    // TODO: re-add long-press touch for flag

    ms::init_quack();
  }
} i;

#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import v;

static void redraw() {
  auto m = v::map();
  ms::grid::instance().load(&*m);
}

static void click() {
  auto [x, y] = ms::calculate_upc().hover;
  ms::grid::instance().click(x, y);
  redraw();
}

static void flag() {
  auto [x, y] = ms::calculate_upc().hover;
  ms::grid::instance().flag(x, y);
  redraw();
}

static void reset_level() {
  ms::grid::instance() = {};
  redraw();
}

extern "C" void casein_init() {
  using namespace casein;
  window_title = "Minesweep";

  handle(KEY_DOWN, K_SPACE, reset_level);
  handle(MOUSE_DOWN, M_LEFT, click);
  handle(MOUSE_DOWN, M_RIGHT, flag);
  handle(GESTURE, G_TAP_1, click);
  // TODO: re-add long-press touch for flag

  v::pc = ms::calculate_upc();

  v::vv::setup([] {
    reset_level();

    v::vv::ss()->frame([] {
      v::vv::ss()->render();
    });
  });
}

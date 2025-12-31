#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import v;

using frame_t = void (*)();
static volatile frame_t frame;

static void redraw() {
  auto m = v::map();
  ms::grid::instance().load(&*m);
  frame = []{};
}

static void click() {
  auto [x, y] = ms::calculate_upc().hover;
  ms::grid::instance().click(x, y);
  frame = redraw;
}

static void flag() {
  auto [x, y] = ms::calculate_upc().hover;
  ms::grid::instance().flag(x, y);
  frame = redraw;
}

static void hover() {
  v::pc = ms::calculate_upc();
  frame = redraw;
}

static void reset_level() {
  ms::grid::instance() = {};
  frame = redraw;
}

static void on(auto e, auto k, void (*fn)()) {
  casein::handle(e, k, [fn] { frame = fn; });
}
static void on(auto e, void (*fn)()) {
  casein::handle(e, [fn] { frame = fn; });
}

extern "C" void casein_init() {
  using namespace casein;
  window_title = "Minesweep";

  on(MOUSE_MOVE, hover);

  on(KEY_DOWN, K_SPACE, reset_level);
  on(MOUSE_DOWN, M_LEFT, click);
  on(MOUSE_DOWN, M_RIGHT, flag);
  on(GESTURE, G_TAP_1, click);
  // TODO: re-add long-press touch for flag

  frame = reset_level;

  v::pc = ms::calculate_upc();

  v::vv::setup([] {
    v::vv::ss()->frame([] {
      frame();
      v::vv::ss()->render();
    });
  });
}

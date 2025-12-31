#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import v;

using frame_t = void (*)();
static volatile frame_t frame;

static ms::grid g_grid {};

static void on(auto e, auto k, void (*fn)()) {
  casein::handle(e, k, [fn] { frame = fn; });
}
static void on(auto e, void (*fn)()) {
  casein::handle(e, [fn] { frame = fn; });
}

static void redraw() {
  auto m = v::map();
  g_grid.load(m);
  frame = []{};
}

static void click() {
  auto [x, y] = ms::calculate_upc().hover;
  g_grid.click(x, y);
  frame = redraw;

  if (g_grid.at(x, y).bomb) {
    using namespace casein;
    reset(MOUSE_MOVE);
    reset_m(MOUSE_DOWN);
    reset_g(GESTURE);
    v::pc.hover = { -1 };
  }
}

static void flag() {
  auto [x, y] = ms::calculate_upc().hover;
  g_grid.flag(x, y);
  frame = redraw;
}

static void hover() {
  v::pc = ms::calculate_upc();
  frame = redraw;
}

static void reset_level() {
  g_grid = {};
  frame = redraw;

  using namespace casein;
  on(MOUSE_MOVE, hover);
  on(MOUSE_DOWN, M_LEFT, click);
  on(MOUSE_DOWN, M_RIGHT, flag);
  on(GESTURE, G_TAP_1, click);
  // TODO: re-add long-press touch for flag
}

extern "C" void casein_init() {
  using namespace casein;
  window_title = "Minesweep";

  on(KEY_DOWN, K_SPACE, reset_level);

  frame = reset_level;

  v::pc = ms::calculate_upc();

  v::vv::setup([] {
    v::vv::ss()->frame([] {
      frame();
      v::vv::ss()->render();
    });
  });
}

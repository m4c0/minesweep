#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import v;

struct game_parameters {
  unsigned grid_size;
  unsigned max_bombs;
};
static constexpr const game_parameters parameters[] {
  { .grid_size = 36, .max_bombs = 36 * 4 },
};

static constexpr const auto grid_size = 36;

using frame_t = void (*)();
static volatile frame_t frame;

static ms::grid g_grid { grid_size };

static void on(auto e, auto k, void (*fn)()) {
  casein::handle(e, k, [fn] { frame = fn; });
}
static void on(auto e, void (*fn)()) {
  casein::handle(e, [fn] { frame = fn; });
}

static auto upc() {
  return ms::calculate_upc(g_grid.grid_size());
}

static void redraw() {
  auto pc = upc();
  if (!g_grid.can_hover(pc.hover.x, pc.hover.y)) pc.hover = -1;
  v::pc = pc;

  auto m = v::map();
  g_grid.load(m);
  frame = []{};
}

static void click() {
  auto [x, y] = upc().hover;
  switch (g_grid.click(x, y)) {
    using enum ms::grid::click_outcome;
    case none: 
    case fill: break;
    case bomb: {
      using namespace casein;
      reset(MOUSE_MOVE);
      reset_m(MOUSE_DOWN);
      reset_g(GESTURE);
      v::pc.hover = { -1 };
      break;
    }
  }

  frame = redraw;
}

static void flag() {
  auto [x, y] = upc().hover;
  g_grid.flag(x, y);
  frame = redraw;
}

static void reset_level() {
  g_grid = { grid_size };
  frame = redraw;

  using namespace casein;
  on(MOUSE_MOVE, redraw);
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

  v::vv::setup([] {
    v::vv::ss()->frame([] {
      frame();
      v::vv::ss()->render();
    });
  });
}

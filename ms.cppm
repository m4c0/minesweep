#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import v;

static constexpr const ms::game_parameters parameters[] {
  { .grid_size = 36, .max_bombs = 36 * 4 },
};

static ms::grid g_grid { parameters[0] };

static auto upc() {
  return ms::calculate_upc(g_grid.grid_size());
}

static void redraw() {
  auto pc = upc();
  if (!g_grid.can_hover(pc.hover.x, pc.hover.y)) pc.hover = -1;
  v::pc = pc;

  auto m = v::map();
  g_grid.load(m);
  v::frame = []{};
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

  v::frame = redraw;
}

static void flag() {
  auto [x, y] = upc().hover;
  g_grid.flag(x, y);
  v::frame = redraw;
}

static void reset_level() {
  g_grid = { parameters[0] };
  v::frame = redraw;

  using namespace casein;
  v::on(MOUSE_MOVE, redraw);
  v::on(MOUSE_DOWN, M_LEFT, click);
  v::on(MOUSE_DOWN, M_RIGHT, flag);
  v::on(GESTURE, G_TAP_1, click);
  // TODO: re-add long-press touch for flag
}

extern "C" void casein_init() {
  using namespace casein;
  window_title = "Minesweep";

  v::on(KEY_DOWN, K_SPACE, reset_level);

  v::frame = reset_level;
  v::setup();
}

#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import v;

static constexpr const auto levels = 2;

static constexpr const ms::game_parameters parameters[levels] {
  { .difficulty = ms::s_easy,  .grid_size = 10, .max_bombs = 8 },
  { .difficulty = ms::s_crazy, .grid_size = 36, .max_bombs = 36 * 4 },
};

static ms::grid g_grid[levels] {
  { parameters[0] },
  { parameters[1] },
};

static unsigned g_diff = 0;
static auto & grid() {
  return g_grid[g_diff];
}

static auto upc() {
  return ms::calculate_upc(grid().grid_size());
}

static void none() {}
static void redraw() {
  auto pc = upc();
  if (!grid().can_hover(pc.hover.x, pc.hover.y)) pc.hover = -1;
  v::pc = pc;

  auto m = v::map();
  grid().draw(m);
  v::frame(none);
}

static void click() {
  auto [x, y] = upc().hover;
  switch (grid().click(x, y)) {
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

  grid().save(g_diff);
  v::frame(redraw);
}

static void flag() {
  auto [x, y] = upc().hover;
  grid().flag(x, y);
  v::frame(redraw);
}

static void setup() {
  v::frame(redraw);

  using namespace casein;
  v::on<MOUSE_MOVE, redraw>();
  v::on<MOUSE_DOWN, M_LEFT, click>();
  v::on<MOUSE_DOWN, M_RIGHT, flag>();
  v::on<GESTURE, G_TAP_1, click>();

  v::on<KEY_DOWN, K_1, [] { g_diff = 0; v::frame(redraw); }>();
  v::on<KEY_DOWN, K_2, [] { g_diff = 1; v::frame(redraw); }>();

  // TODO: click difficulty to switch
  // TODO: add dig/flag modes and UI
  // TODO: detect winning condition
  // TODO: winning animation
}
static void reset_level() {
  grid().reset();
  setup();
}
static void load() {
  for (auto i = 0; i < levels; i++) g_grid[i].load(i);
  setup();
}

extern "C" void casein_init() {
  using namespace casein;
  window_title = "Minesweep";

  v::on<KEY_DOWN, K_SPACE, reset_level>();

  v::push<load>();
  v::setup();
}

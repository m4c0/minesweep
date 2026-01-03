#pragma leco app
export module ms;
import :grid;
import :upc;
import casein;
import file;
import v;

// To be fixed before publishing
// TODO: we can "flag" empty tiles
// TODO: save which diff was last opened
// TODO: right-click on WASM (or even iOS)

static constexpr const auto levels = 4;

static constexpr const ms::game_parameters parameters[levels] {
  { .difficulty = ms::s_easy,   .label_w = 3.0f, .grid_size = 10, .max_bombs =   8 },
  { .difficulty = ms::s_medium, .label_w = 4.5f, .grid_size = 16, .max_bombs =  30 },
  { .difficulty = ms::s_hard,   .label_w = 3.0f, .grid_size = 24, .max_bombs =  64 },
  { .difficulty = ms::s_crazy,  .label_w = 4.0f, .grid_size = 36, .max_bombs = 144 },
};

static ms::grid g_grid[levels] {
  { parameters[0] },
  { parameters[1] },
  { parameters[2] },
  { parameters[3] },
};

static unsigned g_diff = 0;
static auto & grid() {
  return g_grid[g_diff];
}

static auto upc() {
  return ms::calculate_upc(grid().grid_size());
}

static void none() {}

static void game_over() {
  auto pc = upc();
  pc.hover = -1;
  v::pc = pc;

  auto m = v::map();
  grid().draw(m);
}

static void redraw() {
  auto pc = upc();
  if (!grid().can_hover(pc.hover.x, pc.hover.y)) pc.hover = -1;
  v::pc = pc;

  auto m = v::map();
  switch (grid().draw(m)) {
    using enum ms::grid::draw_outcome;
    case none: v::frame(::none); break;
    case won: v::frame(game_over); break;
  }
}

static void save() {
  file::writer f { 99 };
  if (f) f.write<unsigned>(g_diff);
}

static void click() {
  auto [x, y] = upc().hover;
  float lx = x - (grid().grid_size() - 5);
  if (y < 0 && lx > 0 && lx < 5) {
    g_diff = (g_diff + 1) % levels;
    save();
    v::frame(redraw);
    return;
  }

  switch (grid().click(x, y)) {
    using enum ms::grid::click_outcome;
    case none: 
    case fill: break;
    case bomb: v::frame(game_over); break;
  }

  save();
  grid().save(g_diff);

  v::frame(redraw);
}

static void flag() {
  auto [x, y] = upc().hover;
  grid().flag(x, y);
  v::frame(redraw);
}

template<unsigned N>
static void diff() {
  g_diff = N;
  save();
  v::frame(redraw);
}

static void setup() {
  v::frame(redraw);

  using namespace casein;
  v::on<MOUSE_MOVE, redraw>();
  v::on<MOUSE_DOWN, M_LEFT, click>();
  v::on<MOUSE_DOWN, M_RIGHT, flag>();
  v::on<GESTURE, G_TAP_1, click>();

  v::on<KEY_DOWN, K_1, diff<0>>();
  v::on<KEY_DOWN, K_2, diff<1>>();
  v::on<KEY_DOWN, K_3, diff<2>>();
  v::on<KEY_DOWN, K_4, diff<3>>();

  // TODO: add dig/flag modes and UI
  // TODO: winning animation
}
static void reset_level() {
  grid().reset();
  setup();
}
static void load() {
  {
    file::reader f { 99 };
    if (f) g_diff = f.read<unsigned>();
  }

  for (auto i = 0; i < levels; i++) g_grid[i].load(i);
  setup();
}

static void toggle_fullscreen() {
  using namespace casein;
  fullscreen = !fullscreen;
  interrupt(IRQ_FULLSCREEN);
}

extern "C" void casein_init() {
  using namespace casein;
  window_title = "Minesweep";

  v::on<KEY_DOWN, K_SPACE, reset_level>();
  v::on<KEY_DOWN, K_F, toggle_fullscreen>();

  v::push<load>();
  v::setup();
}

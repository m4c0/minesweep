#pragma leco app
export module ms;
import :grid;
import :label;
import :upc;
import :vulkan;
import casein;
import quack;

static ms::grid g_cells{};

static void render() {
  // ms::vulkan::instance().load(&g_cells);
  // ms::label::instance().update_bomb_count(g_cells.bomb_count());
}

static void click() {
  auto [x, y] = ms::pc.sel();
  g_cells.click(x, y);
  render();
}

static void flag() {
  auto [x, y] = ms::pc.sel();
  g_cells.flag(x, y);
  render();
}

static void reset_level() {
  g_cells = {};
  render();
}

static struct init {
  init() {
    using namespace casein;
    set_title("Minesweep");
    handle(CREATE_WINDOW, reset_level);
    handle(KEY_DOWN, K_SPACE, reset_level);
    handle(MOUSE_DOWN, M_LEFT, click);
    handle(MOUSE_DOWN, M_RIGHT, flag);
    handle(GESTURE, G_TAP_1, click);
    // TODO: re-add long-press touch for flag
  }
} i;

export module ms;
import casein;
import quack;

class game_grid : public quack::grid_renderer<36, 36, bool> {
public:
  void reset_level() {
    load_atlas(16, 16, [](quack::u8_rgba *) {});
    fill_uv([](auto b) { return quack::uv{}; });
    fill_colour([](auto b) { return quack::colour{}; });
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.reset_level();
    break;
  case casein::REPAINT:
    r.repaint();
    break;
  case casein::KEY_DOWN:
    switch (e.as<casein::events::key_down>().key()) {
    case casein::K_SPACE:
      r.reset_level();
      break;
    default:
      break;
    }
    break;
  case casein::QUIT:
    r.quit();
    break;
  default:
    break;
  }
}

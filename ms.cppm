#pragma leco app
export module ms;
import :atlas;
import casein;

using namespace ms;

constexpr const auto grid_size = 36;
constexpr const auto max_bombs = grid_size * 4;

class game_grid {
  quack::renderer m_r{1};
  quack::grid_ilayout<grid_size, grid_size, cell> m_grid{&m_r};
  unsigned m_ticks{};

  static constexpr const auto cells = decltype(m_grid)::cells;

  void render() {
    m_grid.fill_uv(uv_filler{});
    m_grid.fill_colour([](const auto &b) {
      if (!b.visible)
        return quack::colour{0, 0, 0, 1};
      if (b.bomb) {
        return quack::colour{0.3, 0, 0, 1};
      } else {
        float f = b.count / 8.0f;
        return quack::colour{0, f * 0.3f, 0, 1};
      }
    });
  }

  void setup_bombs() {
    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = (m_ticks * 115249 ^ m_ticks * 331319) % cells;
      while (m_grid.at(p).bomb) {
        p = (++p * 60493) % cells;
      }
      m_grid.at(p).bomb = true;
    }
  }

  void update_numbers() {
    for (auto y = 0; y < grid_size; y++) {
      for (auto x = 0; x < grid_size; x++) {
        update_numbers_at(x, y);
      }
    }
  }

  void update_numbers_at(unsigned x, unsigned y) {
    if (m_grid.at(x, y).bomb)
      return;

    for (auto dy = -1; dy <= 1; dy++) {
      const auto ny = y + dy;
      if ((ny < 0) || (ny >= grid_size))
        continue;
      for (auto dx = -1; dx <= 1; dx++) {
        const auto nx = x + dx;
        if ((nx < 0) || (nx >= grid_size))
          continue;
        if (m_grid.at(nx, ny).bomb)
          m_grid.at(x, y).count++;
      }
    }
  }

  void build_atlas() { m_r.load_atlas(atlas::width, atlas::height, atlas{}); }

  void fill(unsigned x, unsigned y) {
    auto &p = m_grid.at(x, y);
    if (p.visible)
      return;

    p.visible = true;

    if (p.count != 0 || p.bomb)
      return;

    for (auto dx = -1; dx <= 1; dx++) {
      auto nx = x + dx;
      if (nx < 0 || nx >= grid_size)
        continue;
      for (auto dy = -1; dy <= 1; dy++) {
        auto ny = y + dy;
        if (ny < 0 || ny >= grid_size)
          continue;
        fill(nx, ny);
      }
    }
  }

public:
  void click() {
    m_grid.current_hover().consume([this](auto idx) {
      fill(idx % grid_size, idx / grid_size);
      render();
    });
  }

  void flag() {
    m_grid.current_hover().consume([this](auto idx) {
      auto &g = m_grid.at(idx);
      g.flagged = !g.flagged;
      render();
    });
  }

  void process_event(const casein::event &e) {
    m_r.process_event(e);
    m_grid.process_event(e);
  }

  void reset_level() {
    m_grid.reset_grid();
    setup_bombs();
    update_numbers();
    build_atlas();
    render();
  }

  void repaint() { m_ticks++; }
};

class thread : public voo::casein_thread {
  game_grid m_gg{};

public:
  void create_window(const casein::events::create_window &e) override {
    casein_thread::create_window(e);
    gg.reset_level();
  }
  void key_down(const casein::events::key_down &e) override {
    switch (*e) {
    case casein::K_SPACE:
      gg.reset_level();
      break;
    default:
    }
  }
  void mouse_down(const casein::events::mouse_down &e) override {
    switch (e.button) {
    case casein::M_LEFT:
      gg.click();
      break;
    case casein::M_RIGHT:
      gg.flag();
      break;
    }
  }
  void touch_down(const casein::events::touch_down &e) override {
    if (e.long_press)
      gg.flag();
  }
  void gesture(const casein::events::gesture &e) override {
    if (*e == casein::G_TAP_1)
      gg.click();
  }

  void run() override {
    voo::device_and_queue dq{"winnipeg", native_ptr()};

    voo::one_quad quad{dq};
    vee::command_buffer cb =
        vee::allocate_primary_command_buffer(dq.command_pool());

    vee::pipeline_layout pl = vee::create_pipeline_layout();

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = sw.render_pass(),
          .shaders{
              voo::shader("poc.vert.spv").pipeline_vert_stage(),
              voo::shader("poc.frag.spv").pipeline_frag_stage(),
          },
          .bindings{
              quad.vertex_input_bind(),
          },
          .attributes{
              quad.vertex_attribute(0),
          },
      });

      extent_loop([&] {
        sw.acquire_next_image();
        sw.one_time_submit(dq, cb, [&] {
          auto scb = sw.cmd_render_pass(cb);
          vee::cmd_bind_gr_pipeline(cb, *gp);
          quad.run(scb, 0);
        });
        sw.queue_present(dq);
      });
    }
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  t.handle(e);
}

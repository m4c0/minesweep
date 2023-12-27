#pragma leco app
#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module ms;
import :atlas;
import casein;
import rng;
import vee;
import voo;

using namespace ms;

constexpr const auto grid_size = 36;
constexpr const auto max_bombs = grid_size * 4;
constexpr const auto cells = grid_size * grid_size;

class grid {
  cell m_cells[grid_size * grid_size]{};

public:
  [[nodiscard]] constexpr auto &operator[](unsigned idx) {
    return m_cells[idx];
  }
  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) {
    return m_cells[y * grid_size + x];
  }
};

class thread : public voo::casein_thread {
  grid m_cells{};

  void render() { /*
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
     */
  }

  void setup_bombs() {
    for (auto i = 0; i < max_bombs; i++) {
      unsigned p = rng::rand(cells);
      while (m_cells[p].bomb) {
        p = rng::rand(cells);
      }
      m_cells[p].bomb = true;
    }
  }

  void update_numbers() {
    for (auto y = 0; y < grid_size; y++) {
      for (auto x = 0; x < grid_size; x++) {
        update_numbers_at(x, y);
      }
    }
  }

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) {
    return m_cells.at(x, y);
  }

  void update_numbers_at(unsigned x, unsigned y) {
    if (at(x, y).bomb)
      return;

    for (auto dy = -1; dy <= 1; dy++) {
      const auto ny = y + dy;
      if ((ny < 0) || (ny >= grid_size))
        continue;
      for (auto dx = -1; dx <= 1; dx++) {
        const auto nx = x + dx;
        if ((nx < 0) || (nx >= grid_size))
          continue;
        if (at(nx, ny).bomb)
          at(x, y).count++;
      }
    }
  }

  void fill(unsigned x, unsigned y) {
    auto &p = at(x, y);
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
  void click() {
    /*
    m_grid.current_hover().consume([this](auto idx) {
      fill(idx % grid_size, idx / grid_size);
      render();
    });
    */
  }

  void flag() {
    /*
    m_grid.current_hover().consume([this](auto idx) {
      auto &g = m_grid.at(idx);
      g.flagged = !g.flagged;
      render();
    });
    */
  }

  void reset_level() {
    m_cells = {};
    setup_bombs();
    update_numbers();
    render();
  }

public:
  void create_window(const casein::events::create_window &e) override {
    casein_thread::create_window(e);
    rng::seed();
    reset_level();
  }
  void key_down(const casein::events::key_down &e) override {
    switch (*e) {
    case casein::K_SPACE:
      reset_level();
      break;
    default:
    }
  }
  void mouse_down(const casein::events::mouse_down &e) override {
    switch (*e) {
    case casein::M_LEFT:
      click();
      break;
    case casein::M_RIGHT:
      flag();
      break;
    }
  }
  void touch_down(const casein::events::touch_down &e) override {
    if ((*e).long_press)
      flag();
  }
  void gesture(const casein::events::gesture &e) override {
    if (*e == casein::G_TAP_1)
      click();
  }

  void run() override {
    voo::device_and_queue dq{"winnipeg", native_ptr()};

    voo::one_quad quad{dq};
    voo::h2l_image img{dq, atlas::width, atlas::height};

    auto cb = vee::allocate_primary_command_buffer(dq.command_pool());

    auto dsl = vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});
    auto dpool =
        vee::create_descriptor_pool(1, {vee::combined_image_sampler()});
    auto dset = vee::allocate_descriptor_set(*dpool, *dsl);

    auto smp = vee::create_sampler(vee::nearest_sampler);
    vee::update_descriptor_set(dset, 0, img.iv(), *smp);

    auto pl = vee::create_pipeline_layout({*dsl});

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = sw.render_pass(),
          .shaders{
              voo::shader("ms.vert.spv").pipeline_vert_stage(),
              voo::shader("ms.frag.spv").pipeline_frag_stage(),
          },
          .bindings{
              quad.vertex_input_bind(),
          },
          .attributes{
              quad.vertex_attribute(0),
          },
      });

      atlas{}(static_cast<rgba *>(*(img.mapmem())));

      extent_loop([&] {
        sw.acquire_next_image();
        sw.one_time_submit(dq, cb, [&](auto &pcb) {
          img.run(pcb);

          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
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

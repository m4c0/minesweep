#pragma leco app
#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module ms;
import :grid;
import :upc;
import casein;
import dotz;
import hai;
import vee;
import voo;

using point = dotz::vec2;

ms::upc g_pc{};

class casein_handler : public casein::handler {
  point m_screen_size{};
  point m_mouse_pos{};
  bool m_render{};
  ms::grid m_cells{};

public:
  [[nodiscard]] auto push_consts() {
    ms::upc res{};
    res.update(ms::grid_size, m_mouse_pos, m_screen_size);
    return res;
  }

  void render() { m_render = true; }

  void click() {
    auto [x, y] = push_consts().sel();
    m_cells.click(x, y);
    render();
  }

  void flag() {
    auto [x, y] = push_consts().sel();
    m_cells.flag(x, y);
    render();
  }

  void reset_level() {
    m_cells = {};
    render();
  }

public:
  [[nodiscard]] constexpr bool dirty() noexcept {
    auto r = m_render;
    m_render = false;
    return r;
  }
  [[nodiscard]] constexpr const auto *cells() noexcept { return &m_cells; }

  void create_window(const casein::events::create_window &e) override {
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
  void mouse_move(const casein::events::mouse_move &e) override {
    m_mouse_pos.x = (*e).x;
    m_mouse_pos.y = (*e).y;
    g_pc = push_consts();
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
  void resize_window(const casein::events::resize_window &e) override {
    m_screen_size.x = (*e).width;
    m_screen_size.y = (*e).height;
    g_pc = push_consts();
  }
  void touch_down(const casein::events::touch_down &e) override {
    if ((*e).long_press)
      flag();
  }
  void gesture(const casein::events::gesture &e) override {
    if (*e == casein::G_TAP_1)
      click();
  }
};

class thread : public voo::casein_thread {
  const ms::grid *m_cells;

public:
  void load(const ms::grid *m) { m_cells = m; }

  void run() override {
    voo::device_and_queue dq{"minesweep", native_ptr()};

    voo::one_quad quad{dq};
    voo::h2l_image img{dq, ms::atlas::width, ms::atlas::height};
    voo::h2l_buffer insts{dq, ms::instance_buf_size};

    auto cb = vee::allocate_primary_command_buffer(dq.command_pool());

    auto dsl = vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});
    auto dpool =
        vee::create_descriptor_pool(1, {vee::combined_image_sampler()});
    auto dset = vee::allocate_descriptor_set(*dpool, *dsl);

    auto smp = vee::create_sampler(vee::nearest_sampler);
    vee::update_descriptor_set(dset, 0, img.iv(), *smp);

    auto pl = vee::create_pipeline_layout(
        {*dsl}, {vee::vertex_push_constant_range<ms::upc>()});

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
              vee::vertex_input_bind_per_instance(ms::instance_size),
          },
          .attributes{
              quad.vertex_attribute(0),
              vee::vertex_attribute_vec2(1, 0),
              vee::vertex_attribute_vec4(1, 2 * sizeof(float)),
              vee::vertex_attribute_vec4(1, 6 * sizeof(float)),
          },
      });

      ms::atlas{}(static_cast<ms::rgba_u8 *>(*(img.mapmem())));

      extent_loop([&] {
        if (m_cells != nullptr) {
          m_cells->load(static_cast<ms::inst *>(*(insts.mapmem())));
          m_cells = nullptr;
        }

        sw.acquire_next_image();
        sw.one_time_submit(dq, cb, [&](auto &pcb) {
          img.run(pcb);
          insts.run(pcb);

          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_push_vertex_constants(*scb, *pl, &g_pc);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
          vee::cmd_bind_vertex_buffers(*scb, 1, insts.buffer());
          quad.run(scb, 0, ms::cells);
        });
        sw.queue_present(dq);
      });
    }
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  static casein_handler ch{};
  t.handle(e);
  ch.handle(e);
  if (ch.dirty()) {
    t.load(ch.cells());
  }
}

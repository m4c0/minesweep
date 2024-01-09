#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module ms:vulkan;
import :grid;
import :upc;
import vee;
import voo;

namespace ms {
class vulkan : public voo::casein_thread {
  const upc *m_pc;
  voo::h2l_buffer *m_insts;

public:
  vulkan(const upc *pc) : casein_thread{}, m_pc{pc} {}

  void load(const ms::grid *m) {
    while (!m_insts) {
      // busy wait until the thread actually start
    }
    auto mem = m_insts->mapmem();
    m->load(static_cast<ms::inst *>(*mem));
  }

  [[nodiscard]] bool ready() const noexcept { return m_insts != nullptr; }

  void run() override {
    voo::device_and_queue dq{"minesweep", native_ptr()};

    voo::one_quad quad{dq};
    voo::h2l_image img{dq, ms::atlas::width, ms::atlas::height};
    voo::h2l_buffer insts{dq, ms::instance_buf_size};
    m_insts = &insts;

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
        sw.acquire_next_image();

        insts.submit(dq);

        sw.one_time_submit(dq, cb, [&](auto &pcb) {
          img.run(pcb);

          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_push_vertex_constants(*scb, *pl, m_pc);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
          insts.cmd_bind_vertex_buffer(scb, 1);
          quad.run(scb, 0, ms::cells);
        });
        sw.queue_present(dq);
      });
    }
  }
};
} // namespace ms

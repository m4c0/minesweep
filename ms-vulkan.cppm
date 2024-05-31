#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"
#pragma leco add_shader "ms-label.vert"
#pragma leco add_shader "ms-label.frag"
#pragma leco add_resource "atlas.png"

export module ms:vulkan;
import :grid;
import :upc;
import vee;
import voo;

static void load_insts(voo::h2l_buffer *b) {
  voo::mapmem mem{b->host_memory()};
  ms::grid::instance().load(static_cast<ms::inst *>(*mem));
}
static void load_label(voo::h2l_image *i) {}

static voo::updater<voo::h2l_buffer> *g_insts{};

namespace ms {
struct vulkan : voo::casein_thread {
  static constexpr const auto label_size = 1024;

  void run() {
    voo::device_and_queue dq{"minesweep"};

    voo::one_quad quad{dq};
    auto insts = voo::updater{dq.queue(), &load_insts, dq,
                              unsigned{ms::instance_buf_size}};
    auto label =
        voo::updater{dq.queue(),           &load_label,          dq,
                     unsigned{label_size}, unsigned{label_size}, false};

    voo::sires_image img{"atlas.png", &dq};
    img.run_once();

    auto dsl = vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});
    auto dpool =
        vee::create_descriptor_pool(2, {vee::combined_image_sampler(2)});
    auto dset = vee::allocate_descriptor_set(*dpool, *dsl);

    auto smp = vee::create_sampler(vee::nearest_sampler);
    vee::update_descriptor_set(dset, 0, img.data().iv(), *smp);

    auto l_dset = vee::allocate_descriptor_set(*dpool, *dsl);
    auto l_smp = vee::create_sampler(vee::linear_sampler);
    vee::update_descriptor_set(l_dset, 0, label.data().iv(), *l_smp);

    auto pl = vee::create_pipeline_layout(
        {*dsl}, {vee::vertex_push_constant_range<ms::upc>()});

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = dq.render_pass(),
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
      auto l_gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = dq.render_pass(),
          .depth_test = false,
          .shaders{
              voo::shader("ms-label.vert.spv").pipeline_vert_stage(),
              voo::shader("ms-label.frag.spv").pipeline_frag_stage(),
          },
          .bindings{quad.vertex_input_bind()},
          .attributes{quad.vertex_attribute(0)},
      });

      label.run_once();
      insts.run_once();

      g_insts = &insts;

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          auto pc = calculate_upc();

          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());

          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
          vee::cmd_push_vertex_constants(*scb, *pl, &pc);
          vee::cmd_bind_vertex_buffers(*scb, 1, insts.data().local_buffer());
          quad.run(*scb, 0, ms::cells);

          vee::cmd_bind_gr_pipeline(*scb, *l_gp);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, l_dset);
          quad.run(*scb, 0);
        });
      });
    }
  }
  static auto &instance() {
    static vulkan i{};
    return i;
  }
};

void redraw() {
  if (g_insts) {
    g_insts->run_once();
  } else {
    vulkan::instance();
  }
}

} // namespace ms

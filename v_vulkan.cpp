module v;
import casein;
import vinyl;
import voo;

struct app_stuff;
struct sized_stuff;
vinyl::v<app_stuff, sized_stuff> g;

struct app_stuff {
  voo::device_and_queue dq { "minesweep", casein::native_ptr };
  vee::render_pass rp = voo::single_att_render_pass(dq);
};
struct sized_stuff {
  voo::swapchain_and_stuff sw { g.as()->dq, *g.as()->rp };
};

const int i = [] {
  g.on_frame() = [] {};
  g.setup();

  return 0;
}();

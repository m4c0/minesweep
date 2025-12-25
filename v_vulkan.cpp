module v;
import casein;
import vinyl;
import voo;

struct v::app_stuff : v::base_app_stuff {};
struct v::sized_stuff : v::base_extent_stuff {
  sized_stuff() : base_extent_stuff { vv::as() } {}
};

static void on_frame() {
  v::vv::ss()->frame([] {
    auto cb = v::vv::ss()->sw.command_buffer();

    v::upc pc {};

    v::vv::ss()->clear({ 0, 0, 0, 1 });
    v::vv::as()->ppl.cmd_draw(cb, &pc);
  });
}

const int i = [] {
  v::vv::setup(on_frame);
  return 0;
}();

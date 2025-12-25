module v;
import vinyl;

struct v::app_stuff : v::base_app_stuff {};
struct v::sized_stuff : v::base_extent_stuff {
  sized_stuff() : base_extent_stuff { vv::as() } {}
};

static void on_frame() {
  vv::ss()->frame([] {
    vv::ss()->clear({ 0, 0, 0, 1 });
    vv::as()->ppl.cmd_draw();
  });
}

const int i = [] {
  v::vv::setup(on_frame);
  return 0;
}();

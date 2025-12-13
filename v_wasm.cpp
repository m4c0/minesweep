module v;
import vinyl;

struct app_stuff;
struct sized_stuff;
vinyl::v<app_stuff, sized_stuff> g;

struct app_stuff {
};
struct sized_stuff {
};

const int i = [] {
  g.on_frame() = [] {};
  g.setup();

  return 0;
}();


#pragma leco add_resource "atlas.png"
#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module v;
import casein;
import clay;
import dotz;
import hai;
import sv;
import vinyl;

namespace v {
  export extern struct upc {
    dotz::vec4 client_area;
    dotz::vec2 hover;
  } pc;
  static_assert(sizeof(upc) == 6 * sizeof(float));

  export struct sprite {
    dotz::vec2 pos;
    dotz::vec4 colour;
    unsigned uv;
  };

  struct app_stuff;
  struct sized_stuff;
  export using vv = vinyl::v<app_stuff, sized_stuff>;

  struct app_stuff : vinyl::base_app_stuff {
    clay::das::pipeline<sprite> ppl {{
      .app = this,
      .shader = "ms",
      .texture = "atlas.png",
      .max_instances = 10240,
      .vertex_attributes = clay::buffer<sprite>::vertex_attributes(
          &sprite::pos,
          &sprite::colour,
          &sprite::uv),
      .push_constant = clay::vertex_push_constant<upc>(),
    }};

    app_stuff() : vinyl::base_app_stuff { "Minesweep" } {}
  };
  struct sized_stuff : vinyl::base_extent_stuff {
    sized_stuff() : base_extent_stuff { vv::as() } {}

    void render();
  };

  export class mapper {
  protected:
    decltype(vv::as()->ppl.map()) m = vv::as()->ppl.map();

    void normalise(sprite & s);

  public:
    void push(sprite s) { 
      normalise(s);
      m += s;
    }
  };
  export hai::uptr<mapper> map() { return hai::uptr { new mapper {} }; }

  using frame_t = void (*)();
  export extern volatile frame_t frame;

  export void on(auto e, auto k, void (*fn)()) {
    casein::handle(e, k, [fn] { frame = fn; });
  }
  export void on(auto e, void (*fn)()) {
    casein::handle(e, [fn] { frame = fn; });
  }

  export void setup() {
    vv::setup([] {
      vv::ss()->frame([] {
        frame();
        vv::ss()->render();
      });
    });
  }
};

module : private;

v::upc v::pc {};
volatile v::frame_t v::frame = [] {};

#ifdef LECO_TARGET_WASM
#pragma leco add_impl v_wasm
#else
#pragma leco add_impl v_vulkan
#endif

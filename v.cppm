#pragma leco add_resource "atlas.png"
#pragma leco add_shader "ms.vert"
#pragma leco add_shader "ms.frag"

export module v;
import clay;
import dotz;
import hai;
import sv;
import vinyl;

namespace v {
  export struct upc {
    dotz::vec4 client_area;
    dotz::vec2 hover;
  };
  export struct sprite : upc {
    dotz::vec2 pos;
    dotz::vec4 uvs;
    dotz::vec4 colour;
  };

  struct app_stuff;
  struct sized_stuff;
  using vv = vinyl::v<app_stuff, sized_stuff>;

  struct app_stuff : vinyl::base_app_stuff {
    clay::das::pipeline<sprite> ppl {{
      .app = this,
      .shader = "ms",
      .texture = "atlas.png",
      .max_instances = 10240,
      .vertex_attributes = clay::buffer<sprite>::vertex_attributes(
          &sprite::pos,
          &sprite::uvs,
          &sprite::colour),
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
    upc m_upc {};

    void normalise(sprite & s);

  public:
    void set_upc(upc g) { m_upc = g; }
    void push(sprite s) { 
      static_cast<upc &>(s) = m_upc;
      normalise(s);
      m += s;
    }
  };
  export hai::uptr<mapper> map() { return hai::uptr { new mapper {} }; }
};

module : private;

const int i = [] {
  v::vv::setup([] {
    v::vv::ss()->frame([] {
      v::vv::ss()->render();
    });
  });
  return 0;
}();

#ifdef LECO_TARGET_WASM
#pragma leco add_impl v_wasm
#else
#pragma leco add_impl v_vulkan
#endif

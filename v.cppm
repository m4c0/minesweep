export module v;
import dotz;
import hai;
import sv;

namespace v {
  export struct uvs {};
  export struct sprite {
    dotz::vec2 pos;
    dotz::vec4 uvs;
    dotz::vec4 colour;
  };

  export struct mapper {
    virtual ~mapper() = default;
    virtual void push(sprite s) = 0;
  };
};

#ifdef LECO_TARGET_WASM
#pragma leco add_impl v_wasm
#else
#pragma leco add_impl v_vulkan
#endif

module;
#ifndef LECO_TARGET_WASM
#  define _CRT_SECURE_NO_WARNINGS
#  include <stdio.h>
#endif

export module file;
#ifndef LECO_TARGET_WASM
import buoy;
import jute;
import hay;
#endif

namespace file {
  export class t {
#ifndef LECO_TARGET_WASM
    using ptr_t = hay<FILE *, fopen, fclose>;
    ptr_t m_f;
#endif

  public:
    explicit t(unsigned id);

    explicit operator bool() const;
  };
}

module : private;

namespace file {
#ifdef LECO_TARGET_WASM
  t::t(unsigned id) {}
  t::operator bool() const { return false; }
#else
  static_assert(sizeof(unsigned) == 4);
  static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

  static constexpr auto open(unsigned id) {
    auto file = jute::fmt<"save-%d.dat">(id);
    auto path = buoy::path("m4c0-ms", *file);
    return hay<FILE *, fopen, fclose>(path.begin(), "rb");
  }

  t::t(unsigned id) : m_f { open(id) } {}
  t::operator bool() const { return m_f.data(); }
#endif
}

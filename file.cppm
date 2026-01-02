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
  template<typename T>
  concept pointer = requires (T t) { *t; };
  template<typename T>
  concept not_a_pointer = !pointer<T>;

  template<typename T>
  concept serializable = not_a_pointer<T> && __is_standard_layout(T);

  export class error {};
  export class reader {
#ifndef LECO_TARGET_WASM
    using ptr_t = hay<FILE *, fopen, fclose>;
    ptr_t m_f;
#endif

  public:
    explicit reader(unsigned id);

    explicit operator bool() const;

    void read(void * data, unsigned size);

    template<serializable T>
    void read(T * t) {
      read(t, sizeof(T));
    }

    template<typename T> T read() {
      static_assert(false, "Type is not serialisable");
    }
    template<serializable T>
    T read() {
      T val {};
      read(&val);
      return val;
    }
  };
}

module : private;

namespace file {
#ifdef LECO_TARGET_WASM
  reader::reader(unsigned id) {}
  reader::operator bool() const { return false; }
  // note: client code should never "read" invalid files
  void reader::read(void * data, unsigned size) { throw error {}; }
#else
  static_assert(sizeof(unsigned) == 4);
  static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

  static constexpr auto open(unsigned id) {
    auto file = jute::fmt<"save-%d.dat">(id);
    auto path = buoy::path("m4c0-ms", *file);
    return hay<FILE *, fopen, fclose>(path.begin(), "rb");
  }

  reader::reader(unsigned id) : m_f { open(id) } {}
  reader::operator bool() const { return m_f.data(); }

  void reader::read(void * data, unsigned size) {
    if (size == 0) return;
    if (fread(data, size, 1, m_f) != 1) throw error {};
  }
#endif
}

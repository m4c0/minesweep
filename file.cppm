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

#ifndef LECO_TARGET_WASM
  void close(FILE *);
  using ptr_t = hay<FILE *, fopen, close>;
#endif

  export class error {};
  export class reader {
#ifndef LECO_TARGET_WASM
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

  export class writer {
#ifndef LECO_TARGET_WASM
    ptr_t m_f;
#endif

  public:
    explicit writer(unsigned id);

    explicit operator bool() const;

    void write(const void * data, unsigned size);

    template<serializable T>
    void write(T val) {
      write(&val, sizeof(T));
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

  writer::writer(unsigned id) {}
  writer::operator bool() const { return false; }
  // note: client code should never "write" invalid files
  void writer::write(const void * data, unsigned size) { throw error {}; }
#else
  static_assert(sizeof(unsigned) == 4);
  static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

  void close(FILE * f) {
    if (f) fclose(f); // Windows crashes if file is not open
  }

  static constexpr ptr_t open(unsigned id, const char * mode) {
    auto file = jute::fmt<"save-%d.dat">(id);
    auto path = buoy::path("m4c0-ms", *file);
    return ptr_t { path.begin(), mode };
  }

  reader::reader(unsigned id) : m_f { open(id, "rb") } {}
  reader::operator bool() const { return m_f.data(); }

  void reader::read(void * data, unsigned size) {
    if (size == 0) return;
    if (fread(data, size, 1, m_f) != 1) throw error {};
  }

  writer::writer(unsigned id) : m_f { open(id, "wb") } {}
  writer::operator bool() const { return m_f.data(); }

  void writer::write(const void * data, unsigned size) {
    if (size == 0) return;
    if (fwrite(data, size, 1, m_f) != 1) throw error {};
  }
#endif
}

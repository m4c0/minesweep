export module ms:atlas;

namespace ms {
struct cell {
  unsigned count;
  bool bomb;
  bool visible;
  bool flagged;
};
struct rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

enum sprites {
  s_bomb,
  s_empty,
  s_hidden,
  s_1,
  s_2,
  s_3,
  s_4,
  s_5,
  s_6,
  s_7,
  s_8,
  s_flagged,
  sprite_count
};
struct atlas {
  static constexpr const auto width = 8;
  static constexpr const auto height = 8 * sprite_count;
  static constexpr const auto pixel_count = width * height;
  static constexpr const char pixels[pixel_count + 1] = "        " //
                                                        " * ** * "
                                                        "  ****  "
                                                        " ****** "
                                                        " ****** "
                                                        "  ****  "
                                                        " * ** * "
                                                        "        "
                                                        "        " //
                                                        "        "
                                                        "        "
                                                        "        "
                                                        "        "
                                                        "        "
                                                        "        "
                                                        "        "
                                                        ".......:" //
                                                        ".      :"
                                                        ".      :"
                                                        ".      :"
                                                        ".      :"
                                                        ".      :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  OO  :"
                                                        ".   O  :"
                                                        ".   O  :"
                                                        ".  OOO :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  OOO :"
                                                        ".   OO :"
                                                        ".  O   :"
                                                        ".  OOO :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  OOO :"
                                                        ".   OO :"
                                                        ".    O :"
                                                        ".  OOO :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  O O :"
                                                        ".  OOO :"
                                                        ".    O :"
                                                        ".    O :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  OOO :"
                                                        ".  OO  :"
                                                        ".    O :"
                                                        ".  OOO :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  OOO :"
                                                        ".  O   :"
                                                        ".  OOO :"
                                                        ".  OOO :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ". OOO  :"
                                                        ".   O  :"
                                                        ".   O  :"
                                                        ".   O  :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ".  OOO :"
                                                        ".  O O :"
                                                        ".  OOO :"
                                                        ".  OOO :"
                                                        ".      :"
                                                        ".:::::::"
                                                        ".......:" //
                                                        ".      :"
                                                        ". O*** :"
                                                        ". O*** :"
                                                        ". O    :"
                                                        ". O    :"
                                                        ".      :"
                                                        ".:::::::";

  void operator()(rgba *img) {
    for (auto i = 0; i < pixel_count; i++) {
      switch (pixels[i]) {
      case ' ':
        img[i] = {0, 0, 0, 0};
        break;
      case '*':
        img[i] = {255, 0, 0, 255};
        break;
      case '.':
        img[i] = {64, 64, 64, 255};
        break;
      case ':':
        img[i] = {32, 32, 32, 255};
        break;
      case 'O':
        img[i] = {128, 255, 64, 255};
        break;
      }
    }
  }
};

struct uv {
  float u0, v0;
  float u1, v1;
};
struct uv_filler {
  static constexpr auto uv(unsigned n) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    return ms::uv{0, n * h, 1, (n + 1) * h};
  }

  auto operator()(const cell &c) {
    if (!c.visible)
      return uv(c.flagged ? s_flagged : s_hidden);

    if (c.bomb)
      return uv(s_bomb);

    if (c.count >= 1 && c.count <= 8)
      return uv(s_1 + c.count - 1);

    return uv(s_empty);
  }
};
} // namespace ms

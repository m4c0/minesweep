export module ms:atlas;
import quack;

struct cell {
  unsigned count;
  bool bomb;
};

namespace ms {
enum sprites {
  s_bomb,
  s_empty,
  s_1,
  s_2,
  s_3,
  s_4,
  s_5,
  s_6,
  s_7,
  s_8,
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
                                                        ".:::::::";

  void operator()(quack::u8_rgba *img) {
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
        img[i] = {64, 128, 32, 255};
        break;
      }
    }
  }
};

struct uv_filler {
  static constexpr auto uv(unsigned n) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
  }

  auto operator()(const cell &c) {
    if (c.bomb)
      return uv(s_bomb);

    if (c.count >= 1 && c.count <= 8)
      return uv(s_empty + c.count);

    return uv(s_empty);
  }
};
} // namespace ms

export module ms:atlas;
import quack;

struct cell {
  unsigned count;
  bool bomb;
};

namespace ms {
enum sprites { s_bomb, s_empty, sprite_count };
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
      return uv(0);

    return uv(1);
  }
};
} // namespace ms

export module ms:atlas;
import quack;

struct cell {
  unsigned count;
  bool bomb;
};

namespace ms {
struct atlas {
  static constexpr const auto width = 16;
  static constexpr const auto height = 16;
  static constexpr const auto pixel_count = width * height;

  void operator()(quack::u8_rgba *img) {
    for (auto i = 0; i < width; i++) {
      img[i] = img[width * i] = {64, 64, 64, 255};
    }
  }
};

struct uv_filler {
  auto operator()(const cell &) { return quack::uv{{0, 0}, {1, 1}}; }
};
} // namespace ms

export module ms:atlas;

namespace ms {
struct atlas {
  static constexpr const auto width = 16;
  static constexpr const auto height = 16;
  static constexpr const auto pixel_count = width * height;

  void operator()(auto *img) {
    for (auto i = 0; i < width; i++) {
      img[i] = img[width * i] = {64, 64, 64, 255};
    }
  }
};
} // namespace ms

export module ms:atlas;
import dotz;

namespace ms {
struct cell {
  unsigned count;
  bool bomb;
  bool visible;
  bool flagged;
};

enum sprites {
  s_bomb,
  s_empty,
  s_hidden,
  s_flagged,
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
static constexpr const auto atlas_cols = 1 << 3;
static constexpr const auto atlas_rows = 1 << 2;

struct uv_filler {
  static constexpr auto uv(unsigned n) {
    constexpr const auto w = 1.0f / static_cast<float>(atlas_cols);
    constexpr const auto h = 1.0f / static_cast<float>(atlas_rows);
    constexpr const dotz::vec2 wh{w, h};

    dotz::vec2 a{n % 4, n / 4};
    dotz::vec2 b = a + 1.0f;

    return dotz::vec4 { a * wh, b * wh };
  }

  static constexpr auto uv(const cell &c) {
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

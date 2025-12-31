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

struct uv_filler {
  static constexpr auto uv(unsigned n) { return dotz::ivec2 { n % 4, n / 4 }; }

  static constexpr auto uv(const cell &c) {
    if (!c.visible) return uv(c.flagged ? s_flagged : s_hidden);
    if (c.bomb) return uv(s_bomb);
    if (c.count >= 1 && c.count <= 8) return uv(s_1 + c.count - 1);
    return uv(s_empty);
  }
};
} // namespace ms

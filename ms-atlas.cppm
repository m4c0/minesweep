export module ms:atlas;

namespace ms {
  struct cell {
    unsigned count;
    bool bomb;
    bool visible;
    bool flagged;
  };

  enum sprites {
    s_bomb = 0,
    s_empty,
    s_hidden,
    s_flagged,
    s_1 = 8,
    s_2,
    s_3,
    s_4,
    s_5 = 15,
    s_6,
    s_7,
    s_8,
  };

  constexpr unsigned uv(const cell &c) {
    if (!c.visible) return c.flagged ? s_flagged : s_hidden;
    if (c.bomb) return s_bomb;
    if (c.count >= 1 && c.count <= 4) return s_1 + c.count - 1;
    if (c.count >= 5 && c.count <= 8) return s_5 + c.count - 5;
    return s_empty;
  }
}

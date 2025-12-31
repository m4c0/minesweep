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
    s_label,
    s_1 = 8,
    s_2,
    s_3,
    s_4,
    s_label_0,
    s_5 = 16,
    s_6,
    s_7,
    s_8,
    s_label_4,
    s_label_8 = s_label_4 + 8,
  };

  constexpr unsigned uv(const cell &c) {
    if (!c.visible) return c.flagged ? s_flagged : s_hidden;
    if (c.bomb) return s_bomb;
    if (c.count >= 1 && c.count <= 4) return s_1 + c.count - 1;
    if (c.count >= 5 && c.count <= 8) return s_5 + c.count - 5;
    return s_empty;
  }
  constexpr unsigned uv_label(unsigned n) {
    if (n < 4) return s_label_0 + n;
    if (n < 8) return s_label_4 + n - 4;
    return s_label_8 + n - 8;
  }
}

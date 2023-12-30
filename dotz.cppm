export module dotz;

export namespace dotz {
struct vec2 {
  float x;
  float y;

  constexpr vec2() = default;
  constexpr vec2(float x, float y) : x{x}, y{y} {}
  constexpr vec2(int x, int y)
      : x{static_cast<float>(x)}, y{static_cast<float>(y)} {}

  constexpr vec2(const vec2 &o) = default;
  constexpr vec2(vec2 &&o) = default;
  constexpr vec2 &operator=(const vec2 &o) = default;
  constexpr vec2 &operator=(vec2 &&o) = default;
};
} // namespace dotz

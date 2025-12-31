#pragma leco app

import casein;
import dotz;
import jojo;
import silog;
import stubby;
import traits;
import v;
import voo;

using namespace traits::ints;

static constexpr const auto cols = 1 << 3;
static constexpr const auto rows = 1 << 2;

static constexpr const unsigned image_w = 8 * cols;
static constexpr const unsigned image_h = 8 * rows;
static constexpr const unsigned quad_count = 1 + (rows * cols);

static unsigned g_cursor_x{};
static unsigned g_cursor_y{};
static bool g_cursor_hl{};
static uint32_t g_pixies[image_h][image_w]{};

static void refresh_atlas() {
  auto & img = v::vv::as()->ppl.texture().img;
  auto buf = voo::bound_buffer::create_from_host(image_w * image_h * 4);
  voo::mapmem m { *buf.memory };
  auto * ptr = static_cast<uint32_t *>(*m);
  for (auto y = 0; y < image_h; y++) {
    for (auto x = 0; x < image_w; x++, ptr++) {
      *ptr = g_pixies[y][x];
    }
  }
  voo::copy_buffer_to_image_sync({ image_w, image_h }, *buf.buffer, *img.img);
}

static void refresh_batch() {
  static constexpr dotz::vec4 nrm { 0 };
  static constexpr dotz::vec4 hgl { 1, 0, 0, 1 };

  auto m = v::vv::as()->ppl.map();
  for (auto y = 0U; y < rows; y++) {
    for (auto x = 0U; x < cols; x++) {
      bool hl = y == g_cursor_y && x == g_cursor_x;
      m += {
        .pos { x, y },
        .colour = hl ? hgl : nrm,
        .uv = y * rows + x,
      };
    }
  }
}

static void flip_cursor() {
  g_cursor_hl = !g_cursor_hl;
  refresh_batch();
}

static void down() {
  if (g_cursor_y >= image_h - 1)
    return;

  g_cursor_y++;
  refresh_batch();
}
static void up() {
  if (g_cursor_y == 0)
    return;

  g_cursor_y--;
  refresh_batch();
}
static void left() {
  if (g_cursor_x == 0)
    return;

  g_cursor_x--;
  refresh_batch();
}
static void right() {
  if (g_cursor_x >= image_w - 1)
    return;

  g_cursor_x++;
  refresh_batch();
}

static void colour(uint32_t c) {
  g_pixies[g_cursor_y][g_cursor_x] = c;
  refresh_atlas();
}
static void colour_1() { colour(0x0); }
static void colour_2() { colour(0xFF0000FF); }
static void colour_3() { colour(0xFF3F3F3F); }
static void colour_4() { colour(0xFF1F1F1F); }
static void colour_5() { colour(0xFF3FFF7F); }

static void save() {
  // TODO: improve safety with a hai::array<pixel> and X/Y accessors
  auto *pix = reinterpret_cast<stbi::pixel *>(g_pixies);
  stbi::write_rgba_unsafe("atlas.png", image_w, image_h, pix);
  silog::log(silog::info, "Atlas saved");
}

extern "C" void casein_init() {
  using namespace casein;

  v::on(KEY_DOWN, K_DOWN, down);
  v::on(KEY_DOWN, K_UP, up);
  v::on(KEY_DOWN, K_LEFT, left);
  v::on(KEY_DOWN, K_RIGHT, right);
  v::on(KEY_DOWN, K_ENTER, save);
  v::on(KEY_DOWN, K_1, colour_1);
  v::on(KEY_DOWN, K_2, colour_2);
  v::on(KEY_DOWN, K_3, colour_3);
  v::on(KEY_DOWN, K_4, colour_4);
  v::on(KEY_DOWN, K_5, colour_5);

  v::pc = v::upc {
    .client_area { image_w, image_h, image_w / 2.0f, image_h / 2.0f },
  };
  v::frame = [] {
    refresh_atlas();
    refresh_batch();
  };
  v::setup();

  auto img = stbi::load(jojo::slurp("atlas.png"));
  if (img.width > image_w) silog::error("image too large");
  if (img.height > image_h) silog::error("image too tall");
  if (img.num_channels != 4) silog::error("image is not RGBA");

  auto * d = reinterpret_cast<uint32_t *>(*img.data);
  for (auto y = 0; y < img.height; y++) {
    for (auto x = 0; x < img.width; x++) {
      g_pixies[y][x] = *d++;
    }
  }
}

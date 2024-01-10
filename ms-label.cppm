#pragma leco add_resource "VictorMono-Regular.otf"
export module ms:label;
import :vulkan;
import casein;
import traits;
import what_the_font;

namespace ms {
class label {
  static constexpr const auto font_name = "VictorMono-Regular.otf";
  static constexpr const auto font_size = 36;
  static constexpr const auto img_size = vulkan::label_size;

  ms::vulkan *m_ui;
  wtf::library m_wtf{};
  wtf::face m_face = m_wtf.new_face(font_name, font_size);

public:
  label(vulkan *ui) : m_ui{ui} {}

  void update_bomb_count(int n) {
    char buf[20]{"Bombs: "};
    auto *p = buf + 7;
    if (n < 0) {
      *p++ = '-';
      n *= -1;
    }
    while (n > 0) {
      *p++ = '0' + n % 10;
      n /= 10;
    }
    traits::size_t sz = p - buf;

    auto m = m_ui->map_label();
    auto *img = static_cast<unsigned char *>(*m);
    for (auto i = 0; i < img_size * img_size; i++) {
      img[i] = 0;
    }
    m_face.shape_en({buf, sz}).draw(img, img_size, img_size, 0, font_size);
  }
};
} // namespace ms

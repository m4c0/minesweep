#pragma leco add_resource "VictorMono-Regular.otf"
export module ms:label;
import :vulkan;
import casein;
import what_the_font;

namespace ms {
class label : public casein::handler {
  static constexpr const auto font_name = "VictorMono-Regular.otf";
  static constexpr const auto font_size = 36;
  static constexpr const auto img_size = vulkan::label_size;

  ms::vulkan *m_ui;
  wtf::library m_wtf{};
  wtf::face m_face = m_wtf.new_face(font_name, font_size);

public:
  label(vulkan *ui) : handler{}, m_ui{ui} {}

  void create_window(const casein::events::create_window &e) override {
    auto m = m_ui->map_label();
    auto *img = static_cast<unsigned char *>(*m);
    m_face.shape_en("Bombs: 1234").draw(img, img_size, img_size, 0, font_size);
  }
};
} // namespace ms

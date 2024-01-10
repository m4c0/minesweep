export module ms:label;
import :vulkan;
import casein;
import what_the_font;

namespace ms {
class label : public casein::handler {
  ms::vulkan *m_ui;

public:
  label(vulkan *ui) : handler{}, m_ui{ui} {}

  void create_window(const casein::events::create_window &e) override {
    auto m = m_ui->map_label();
  }
};
} // namespace ms

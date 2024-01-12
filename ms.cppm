#pragma leco app
export module ms;
import :events;
import :label;
import :upc;
import :vulkan;
import casein;

extern "C" void casein_handle(const casein::event &e) {
  static ms::vulkan t{};
  static ms::label lbl{&t};
  static ms::casein_handler ch{&t, &lbl};

  ms::pc_handler::instance().handle(e);
  t.handle(e);
  ch.handle(e);
}

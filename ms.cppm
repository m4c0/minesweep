#pragma leco app
export module ms;
import :events;
import :upc;
import :vulkan;
import casein;

extern "C" void casein_handle(const casein::event &e) {
  static ms::pc_handler pc{};
  static ms::vulkan t{*pc};
  static ms::casein_handler ch{*pc, &t};

  pc.handle(e);
  t.handle(e);
  ch.handle(e);
}

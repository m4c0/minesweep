#pragma leco app
export module ms;
import :events;
import :upc;
import :vulkan;
import casein;

extern "C" void casein_handle(const casein::event &e) {
  static ms::pc_handler pc{};
  static ms::vulkan t{*pc};
  static ms::casein_handler ch{*pc};

  pc.handle(e);
  ch.handle(e);

  if (t.ready())
    if (ch.dirty()) {
      t.load(ch.cells());
    }
  t.handle(e);
}

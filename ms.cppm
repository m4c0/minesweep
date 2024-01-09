#pragma leco app
export module ms;
import :events;
import :upc;
import :vulkan;
import casein;

extern "C" void casein_handle(const casein::event &e) {
  static ms::vulkan t{};
  static ms::casein_handler ch{};
  static ms::pc_handler pc{};

  pc.handle(e);
  t.set_pc(*pc);
  ch.set_pc(*pc);

  ch.handle(e);

  if (t.ready())
    if (ch.dirty()) {
      t.load(ch.cells());
    }
  t.handle(e);
}

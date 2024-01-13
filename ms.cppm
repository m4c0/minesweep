#pragma leco app
export module ms;
import :events;
import :upc;
import :vulkan;
import casein;
import quack;

extern "C" void casein_handle(const casein::event &e) {
  quack::mouse_tracker::instance().handle(e);
  ms::pc_handler::instance().handle(e);
  ms::vulkan::instance().handle(e);
  ms::casein_handler::instance().handle(e);
}

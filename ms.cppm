#pragma leco app
export module ms;
import :events;
import :upc;
import :vulkan;
import casein;

extern "C" void casein_handle(const casein::event &e) {
  ms::pc_handler::instance().handle(e);
  ms::vulkan::instance().handle(e);
  ms::casein_handler::instance().handle(e);
}

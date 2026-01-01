module v;
import gelo;

void v::sized_stuff::render() {
  using namespace gelo;

  if (!vv::as()->ppl) return;

  static unsigned u_client_area = 0;
  static unsigned u_hover = 0;
  if (!u_client_area) {
    auto p = v::vv::as()->ppl.program();
    u_client_area = get_uniform_location(p, "pc.client_area");
    u_hover = get_uniform_location(p, "pc.hover");
  }

  uniform4fv(u_client_area, pc.client_area);
  uniform2fv(u_hover, pc.hover);

  vv::ss()->clear({ 0, 0, 0, 1 });
  vv::as()->ppl.cmd_draw();
}
void v::mapper::normalise(v::sprite & s) {}

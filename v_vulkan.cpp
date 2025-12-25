module v;
import casein;
import vinyl;
import voo;

void v::sized_stuff::render() {
  auto cb = v::vv::ss()->sw.command_buffer();

  v::upc pc {};

  auto rp = v::vv::ss()->clear({ 0, 0, 0, 1 });
  v::vv::as()->ppl.cmd_draw(cb, &pc);
}
void v::mapper::normalise(v::sprite & s) {}

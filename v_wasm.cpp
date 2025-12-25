module v;

void v::sized_stuff::render() {
  if (!vv::as()->ppl) return;

  vv::ss()->clear({ 0, 0, 0, 1 });
  vv::as()->ppl.cmd_draw();
}

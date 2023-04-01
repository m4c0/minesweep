#include "../casein/build.hpp"
#include "../quack/build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto m = unit::create<mod>("ms");
  m->add_wsdep("casein", casein());
  m->add_wsdep("quack", quack());
  m->add_part("atlas");

  auto a = unit::create<app>("minesweep");
  a->add_ref(m);
  return run_main(a, argc, argv);
};

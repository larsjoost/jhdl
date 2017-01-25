// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
using vhdl::std::env::finish;
SC_MODULE(test)
{
void noname0() {
finish(1);
}
public:
SC_CTOR(test) {
  SC_METHOD(noname0);
}
};
int sc_main(int argc, char* argv[]) {
  test a;
}

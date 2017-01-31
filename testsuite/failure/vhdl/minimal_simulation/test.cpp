// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
using namespace vhdl;
using namespace vhdl::STANDARD;
using vhdl::STD::ENV::FINISH;
SC_MODULE(test)
{
void noname0() {
FINISH(1);
}
public:
SC_CTOR(test) {
  SC_METHOD(noname0);
}
};
int sc_main(int argc, char* argv[]) {
  test a;
}

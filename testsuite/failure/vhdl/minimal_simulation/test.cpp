// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
using namespace STANDARD;
using STD::ENV::FINISH;
SC_MODULE(TEST)
{
void noname0() {
REPORT("Finish with error", ERROR);
FINISH(1);
}
public:
SC_CTOR(TEST) {
  SC_METHOD(noname0);
}
};
}
int sc_main(int argc, char* argv[]) {
  vhdl::TEST a;
}

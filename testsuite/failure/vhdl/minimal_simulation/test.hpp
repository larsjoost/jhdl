// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
using namespace STANDARD;
using STD::ENV::FINISH;
SC_MODULE(TEST) {
void noname0() {
while(true) {
report("Finish with error", ERROR);
FINISH(1);
}
}
public:
TEST() {
  SC_METHOD(&TEST::noname0);
}
};
}

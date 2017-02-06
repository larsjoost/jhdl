// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
using namespace STANDARD;
using STD::ENV::FINISH;
SC_MODULE(TEST) {
void noname0() {
while(true) {
report("Test is almost done", FAILURE);
FINISH(0);
}
}
public:
TEST() {
  SC_METHOD(&TEST::noname0);
}
};
}

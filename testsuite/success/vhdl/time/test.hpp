// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
using namespace STANDARD;
using STD::ENV::FINISH;
SC_MODULE(TEST) {
void noname0() {
while(true) {
/*
    variable t : time;
             ^
*/
TIME T;
/*
    t := 10 ns;
    ^
*/
T = physical(10, NS);
report(concat("time = ", TIME::IMAGE(T)), NOTE);
report(concat("now = ", TIME::IMAGE(NOW)), NOTE);
FINISH(0);
}
}
public:
TEST() {
  SC_METHOD(&TEST::noname0);
}
};
}

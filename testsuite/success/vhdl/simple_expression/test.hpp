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
    variable x : integer;
             ^
*/
INTEGER X;
/*
    x := 10 + 4;
    ^
*/
X = 10 + 4;
report(concat("x = ", INTEGER::IMAGE(X)), NOTE);
FINISH(0);
}
}
public:
TEST() {
  SC_METHOD(&TEST::noname0);
}
};
}

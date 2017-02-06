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
    x := 10;
    ^
*/
X = 10;
if  (equal(X, 10)) {
report("Success", NOTE);
} else {
report("Failure", FAILURE);
}
FINISH(0);
}
}
public:
TEST() {
  SC_METHOD(&TEST::noname0);
}
};
}

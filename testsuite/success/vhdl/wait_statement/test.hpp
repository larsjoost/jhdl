// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
using namespace STANDARD;
using STD::ENV::FINISH;
SC_MODULE(TEST) {
void noname0() {
while(true) {
for (auto I :  INTEGER(0, 3)) {
report(concat("(1) now = ", TIME::IMAGE(NOW)), NOTE);
wait(5);
}
FINISH(0);
}
}
void noname1() {
while(true) {
for (auto I :  INTEGER(0, 3)) {
report(concat("(2) now = ", TIME::IMAGE(NOW)), NOTE);
wait(2);
}
wait(1000);
}
}
public:
TEST() {
  SC_METHOD(&TEST::noname0);
  SC_METHOD(&TEST::noname1);
}
};
}

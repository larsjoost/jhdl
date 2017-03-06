// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
  using namespace STANDARD;
  using STD::ENV::FINISH;
  
  SC_MODULE(TEST) {
  public:
    INTEGER A; INTEGER B; INTEGER Q;
    class line11 : public sc_thread {
      TEST* p;
    public:
      line11(TEST* parent) : p(parent) {};
      void process() {
        wait([&](){return A.EVENT() || B.EVENT();});
        // line 11:   q <= a + b;
        Q = A + B;
      }
    };
    public:
      SC_CTOR(TEST) {
      SC_THREAD(new line11(this));
      }
  };
}
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
  using namespace STANDARD;
  using STD::ENV::FINISH;
  
  SC_MODULE(TEST2) {
  public:
    // line 24:   signal a : integer;
    // line 24:   signal a : integer;
    sc_signal<INTEGER> A;
    // line 25:   signal b : integer;
    // line 25:   signal b : integer;
    sc_signal<INTEGER> B;
    // line 26:   signal q : integer;
    // line 26:   signal q : integer;
    sc_signal<INTEGER> Q;
    class noname0 : public sc_thread {
      TEST2* p;
    public:
      noname0(TEST2* parent) : p(parent) {};
      void process() {
        // line 38:     a <= 1;
        p->A = 1;
        // line 39:     b <= 2;
        p->B = 2;
        wait(1);
        if  (p->Q != 3) {
        report(concat("Q = ", concat(INTEGER::IMAGE(p->Q), ", but expected = 3")), FAILURE);
        }
// warning in file test.vhdl at 44, 4: Could not find function FINISH declaration. Cannot associate arguments.
//     finish(0);
//     ^
        FINISH(0);
      }
    };
    public:
      SC_CTOR(TEST2) {
      SC_THREAD(new noname0(this));
      }
  };
}

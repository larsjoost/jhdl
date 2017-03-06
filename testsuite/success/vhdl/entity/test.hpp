// Filename : test.vhdl
#include "systemc.h"
#include "vhdl.h"
namespace vhdl {
  using namespace STANDARD;
  using STD::ENV::FINISH;
  
  SC_MODULE(TEST) {
  public:
    sc_in<INTEGER> A; sc_in<INTEGER> B; sc_out<INTEGER> Q;
    class line11 : public sc_thread {
      TEST* p;
    public:
      line11(TEST* parent) : p(parent) {};
      void process() {
        wait([&](){return p->A.EVENT() || p->B.EVENT();});
        // line 11:   q <= a + b;
        p->Q = p->A + p->B;
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
    // line 24:   signal a_i : integer;
    // line 24:   signal a_i : integer;
    sc_signal<INTEGER> A_I;
    // line 25:   signal b_i : integer;
    // line 25:   signal b_i : integer;
    sc_signal<INTEGER> B_I;
    // line 26:   signal q_i : integer;
    // line 26:   signal q_i : integer;
    sc_signal<INTEGER> Q_I;
    class noname0 : public sc_thread {
      TEST2* p;
    public:
      noname0(TEST2* parent) : p(parent) {};
      void process() {
        // line 38:     a_i <= 1;
        p->A_I = 1;
        // line 39:     b_i <= 2;
        p->B_I = 2;
        wait(1);
        if  (p->Q_I != 3) {
          report(concat("Q = ", concat(INTEGER::IMAGE(p->Q_I), ", but expected = 3")), FAILURE);
        }
// warning in file test.vhdl at 44, 4: Could not find function FINISH declaration. Cannot associate arguments.
//     finish(0);
//     ^
        FINISH(0);
      }
    };
    public:
      SC_CTOR(TEST2) {
      // line 30:   test_1: entity work.test(rtl)
      auto TEST_1 = TEST("TEST_1");
      TEST_1.A.bind(A_I); TEST_1.B.bind(B_I); TEST_1.Q.bind(Q_I);
      SC_THREAD(new noname0(this));
      }
  };
}

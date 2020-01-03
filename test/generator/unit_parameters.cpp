
#include "parameters.hpp"

class UnitParameters {

public:
  
  void test_hierarchy() {
    generator::parameters parm;
    parm.printHierarchy();
    parm.openHierarchy("first");
    parm.println(generator::parameters::Area::DECLARATION, "Declaration test");
    parm.printHierarchy();
    parm.closeHierarchy();
    parm.printHierarchy();
    parm.close();
    parm.printHierarchy();
  }
};


int main(int argv, char** argc) {
  UnitParameters u;
  u.test_hierarchy();
}

  

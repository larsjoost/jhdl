#include <cassert>

#include "systemc.hpp"

namespace generator {

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      functionStart("objectDeclaration");
      assert(v->identifier);
      std::string name = v->identifier->toString(true);
      parm.database.addObject(name, v->objectType);
      std::string type = name + "_type";
      type = subtypeIndication(parm, type, v->type) + "<>";
      std::string init = "";
      if (v->initialization) {
        init = expressionToString(parm, v->initialization->value);
      }
      callback(name, type, init, v->objectType, v->direction);
      functionEnd("objectDeclaration");
    }
  }

}

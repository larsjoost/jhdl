#include <cassert>

#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      functionStart("objectDeclaration");
      assert(v->identifier);
      std::string name = v->identifier->toString(true);
      std::string type = name + "_type";
      ast::ObjectValueContainer value;
      type = subtypeIndication(parm, value, type, v->type);
      database.add(v->objectType, name, value);
      std::string init = "";
      if (v->initialization) {
        ExpressionParser expr(&database);
        init = expr.toString(v->initialization->value, value);
      }
      callback(name, type, init, v->objectType, v->direction);
      functionEnd("objectDeclaration");
    }
  }

}

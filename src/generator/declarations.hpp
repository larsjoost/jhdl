#include <cassert>

#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      debug.functionStart("objectDeclaration");
      assert(v->identifier);
      std::string name = v->identifier->toString(true);
      std::string type = name + "_type";
      DatabaseResult database_result;
      type = subtypeIndication(parm, database_result, type, v->type);
      database.add(v->objectType, name, database_result.object->type);
      std::string init = "";
      if (v->initialization) {
        ExpressionParser expr(&database);
        init = expr.toString(v->initialization->value, database_result.object->type);
      }
      callback(name, type, init, v->objectType, v->direction);
      debug.functionEnd("objectDeclaration");
    }
  }

}

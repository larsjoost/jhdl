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
      DatabaseResult database_result;
      bool subtype;
      type = subtypeIndication(parm, database_result, type, v->type, subtype);
      database.add(v->objectType, name, database_result.object->type);
      if (!subtype) {
        type = database.globalName(database_result, type);
      }
      std::string init = "";
      if (v->initialization) {
        ExpressionParser expr(&database);
        init = expr.toString(v->initialization->value, database_result.object->type);
      }
      callback(name, type, init, v->objectType, v->direction);
      functionEnd("objectDeclaration");
    }
  }

}

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
      debug.debug("Name = " + name + ", type = " + type);
      DatabaseResult database_result;
      type = subtypeIndication(parm, database_result, type, v->type);
      a_database.add(v->objectType, name, database_result.object->type);
      std::string init = "";
      if (v->initialization) {
        init = a_expression.toString(v->initialization->value, database_result.object->type);
        debug.debug("Init = " + init, true);
      }
      callback(name, type, init, v->objectType, v->direction);
      debug.functionEnd("objectDeclaration");
    }
  }

}

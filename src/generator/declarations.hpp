#include <cassert>

#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::ObjectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback, std::string local_prefix,
                                  bool database_enable) {
    if (v) {
      debug.functionStart("ObjectDeclaration");
      printSourceLine(parm, v->text);
      DatabaseResult database_result;
      std::string type_name = v->type->name->toString(true);
      if (a_database.findOne(database_result, type_name, ast::ObjectType::TYPE)) { 
        std::string global_type_name = a_name_converter.getName(database_result, true);
        std::string factory_name = a_name_converter.getPrefix(database_result, "::", "::") + "factory_" + type_name;
        if (v->type->range) {
          std::string left, right;
          rangeToString(v->type->range, left, right, database_result.object->type);
          factory_name += ".create(" + left + ", " + right + ")";
        } else {
          factory_name += ".create()";
        }
        for (ast::SimpleIdentifier& id : v->identifiers.list) {
          std::string name = id.toString(true);
          debug.debug("Name = " + name + ", type = " + type_name);
          if (database_enable) {a_database.add(v->objectType, name, database_result.object->type);}
          std::string init = "";
          if (v->initialization) {
            init = a_expression.toString(v->initialization->value, database_result.object->type);
            debug.debug("Init = " + init, true);
          }
          callback(name, type_name, init, factory_name, v->objectType, v->direction);
        }
      } else {
        exceptions.printError("Could not find type \"" + type_name + "\"", v->text);
      }
      debug.functionEnd("ObjectDeclaration");
    }
  }

  template<typename Func>
  void SystemC::PrintTypeObject(parameters& parm, const std::string& name, Func func) {
    debug.functionStart("PrintTypeObject");
    DefineObject(parm, false, name, "Factory", "", NULL, NULL, NULL, func,
                 [](parameters& parm) {}, false);

    debug.functionEnd("PrintTypeObject");
  }

}

#include <cassert>

#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

    /*
  vhdl:
    <name>        <range>
    integer range 0 to 10;
  systemc:
    struct TYPE_T_range { int left = 0; int right = 4; };
    template<class T = TYPE_T_range>
    using TYPE_T = INTEGER<T>;
  */
  template<typename Func>
  std::string SystemC::Subtype(parameters& parm, DatabaseResult& database_result,
                               std::string& name, ast::SubtypeIndication* t,
                               Func func) {
    debug.functionStart("Subtype");
    assert(t);
    std::string typeName = t->name->toString(true);
    debug.debug("Search for " + typeName);
    if (a_database.findOne(database_result, typeName, ast::ObjectType::TYPE)) { 
      typeName = a_database.namePrefix(database_result) + typeName;
      debug.debug("Found typeName = " + typeName);
      if (t->range) {
        printSubtype(parm, name, t->range, typeName, database_result.object->type);
      }
      typeName = func(t->range, name, typeName);
      typeName += "<>";
    } else {
      throw Database::ObjectNotFoundException("Could not find type \"" + typeName + "\"", &t->name->text);
    }
    debug.debug("typeName = " + typeName);
    debug.functionEnd("Subtype");
    return typeName;
  }

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback, std::string local_prefix,
                                  bool database_enable) {
    if (v) {
      debug.functionStart("objectDeclaration");
      printSourceLine(parm, v->text);
      for (ast::SimpleIdentifier& id : v->identifiers.list) {
        std::string name = id.toString(true);
        std::string type = name + "_type";
        debug.debug("Name = " + name + ", type = " + type);
        DatabaseResult database_result;
        auto func = [&](ast::RangeType* r, std::string& name, std::string& type_name) {
          return (r ? local_prefix + name : type_name);
        };
        try {
          type = Subtype(parm, database_result, type, v->type, func);
          if (database_enable) {a_database.add(v->objectType, name, database_result.object->type);}
          std::string init = "";
          if (v->initialization) {
            init = a_expression.toString(v->initialization->value, database_result.object->type);
            debug.debug("Init = " + init, true);
          }
          callback(name, type, init, v->objectType, v->direction);
        } catch (Database::ObjectNotFoundException o) {
          o.print();
        }
      }
      debug.functionEnd("objectDeclaration");
    }
  }

}

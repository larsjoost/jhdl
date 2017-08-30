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
        type_name = a_name_converter.GetName(database_result, false);
        std::string factory_arguments;
        if (v->type->range) {
          std::string left, right;
          ast::ObjectValueContainer i = database_result.object->type;
          try {
            i = database_result.object->type.GetArgument();
          } catch (ast::ObjectValueContainer::IndexOutOfRange e) {}
          rangeToString(v->type->range, left, right, i);
          factory_arguments = left + ", " + right;
        }
        std::string factory_name = a_name_converter.GetName(database_result, true, factory_arguments);
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
    DefineObject(parm, false, name, ast::ObjectType::FACTORY, "", NULL, NULL, NULL, func,
                 [](parameters& parm) {}, false, false);

    debug.functionEnd("PrintTypeObject");
  }

  template<typename Func>
  void SystemC::PrintFactory(parameters& parm, const std::string& name, Func func) {
    std::string left;
    std::string right;
    auto f = [&](parameters& parm) {
      func(parm, left, right);
      bool arguments_exists = !left.empty();
      std::string arguments = arguments_exists ? "(" + left+ ", " + right+ ")" : "";
      parm.println(parameters::Area::DECLARATION, name + " create() {");
      parm.println(parameters::Area::DECLARATION, name + " x" + arguments + ";");
      parm.println(parameters::Area::DECLARATION, "return x;");
      parm.println(parameters::Area::DECLARATION, "}");
      parm.println(parameters::Area::DECLARATION, "template <typename T>");
      parm.println(parameters::Area::DECLARATION, name + " create(T left, T right) {");
      parm.println(parameters::Area::DECLARATION, name + " x(left, right);");
      parm.println(parameters::Area::DECLARATION, "return x;");
      parm.println(parameters::Area::DECLARATION, "}");
    };
    PrintTypeObject(parm, name, f);
    parm.println(parameters::Area::DECLARATION, "Factory_" + name + " factory_" + name + " = " + "Factory_" + name + "(this);");
  }


}

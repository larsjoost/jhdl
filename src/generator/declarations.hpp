#include <cassert>

#include "systemc.hpp"
#include "expression/expression.hpp"
#include "parameters.hpp"
#include "database/general.hpp"

namespace generator {

  template<typename Func>
  void SystemC::ObjectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback, std::string local_prefix,
                                  bool database_enable) {
    if (v) {
      std::string type_name = v->type->name->toString(true);
      debug.functionStart("ObjectDeclaration(type_name = " + type_name + ")");
      DatabaseResult database_result;
      if (parm.findOne(database_result, type_name, ast::ObjectType::TYPE)) { 
        type_name = NameConverter::getName(parm, database_result, false);
        std::string factory_arguments;
        if (v->type->range) {
          std::string left, right;
          ast::ObjectValueContainer i = database_result.object->type;
          try {
            i = database_result.object->type.GetArgument();
          } catch (ast::ObjectValueContainer::IndexOutOfRange e) {}
          rangeToString(parm, v->type->range, left, right, i);
          factory_arguments = left + ", " + right;
        }
        std::string factory_name = NameConverter::getName(parm, database_result, true, factory_arguments);
        for (ast::SimpleIdentifier& id : v->identifiers.list) {
          std::string name = id.toString(true);
          debug.debug("Name = " + name + ", type = " + type_name);
          if (database_enable) {parm.addObject(v->objectType, name, database_result.object->type);}
          std::string init = "";
          if (v->initialization) {
            init = a_expression.toString(parm, v->initialization->value, database_result.object->type);
            debug.debug("Init = " + init, true);
          }
          callback(name, type_name, init, factory_name, v->objectType, v->direction);
        }
      } else {
        exceptions.printError("Could not find type \"" + type_name + "\"", v->text);
      	if (a_verbose) {parm.printDatabase();}
      }
      debug.functionEnd("ObjectDeclaration");
    }
  }

  template<typename Func>
  void SystemC::PrintTypeObject(parameters& parm, const std::string& name, Func func) {
    debug.functionStart("PrintTypeObject(name = " + name + ")");
    std::string class_description = "struct " + ObjectName(ast::ObjectType::FACTORY, name);
    defineObject(parm, false, name, ast::ObjectType::FACTORY, class_description, NULL, NULL, NULL, func,
                 [](parameters& parm) {}, false, false);

    debug.functionEnd("PrintTypeObject");
  }

  template<typename Func>
  void SystemC::PrintFactory(parameters& parm, const std::string& name, Func func) {
    debug.functionStart("PrintFactory(name = " + name + ")");
    std::string left;
    std::string right;
    auto f = [&](parameters& parm) {
      func(parm, left, right);
      bool arguments_exists = !left.empty();
      std::string arguments = arguments_exists ? "(" + left+ ", " + right+ ")" : "";
      parm.addClassContents(name + " create() {");
      parm.addClassContents(name + " x" + arguments + ";");
      parm.addClassContents("return x;");
      parm.addClassContents("}");
      parm.addClassContents("template <typename T>");
      parm.addClassContents(name + " create(T left, T right) {");
      parm.addClassContents(name + " x(left, right);");
      parm.addClassContents("return x;");
      parm.addClassContents("}");
    };
    PrintTypeObject(parm, name, f);
    std::string factory_name = ObjectName(ast::ObjectType::FACTORY, name);
    parm.addClassBottom(factory_name + " factory_" + name + " = " + factory_name + "(this);");
    debug.functionEnd("PrintFactory");
  }


}

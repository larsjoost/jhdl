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
          std::string left, right, ascending;
          ast::ObjectValueContainer i = database_result.object->type;
          try {
            i = database_result.object->type.GetArgument();
          } catch (ast::ObjectValueContainer::IndexOutOfRange e) {}
          rangeToString(parm, v->type->range, left, right, ascending, i);
          factory_arguments = left + ", " + right + ", " + ascending;
        }
        std::string factory_name = NameConverter::getName(parm, database_result, true, factory_arguments);
        for (ast::SimpleIdentifier& id : v->identifiers.list) {
          std::string name = id.toString(true);
          debug.debug("Name = " + name + ", type = " + type_name);
          if (database_enable) {parm.addObject(v->objectType, name, database_result.object->type);}
          std::string init = "";
          if (v->initialization) {
            auto sensitivity_list_callback = 
	      [&](DatabaseResult& object, std::string& name_extension) {};
	    init = a_expression.assignmentString(parm, v->initialization->value, database_result.object->type,
						 sensitivity_list_callback, name);
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
    debug.functionStart("PrintTypeObject(name = " + name + ")", false,  __FILE__, __LINE__);
    defineObject(parm, false, name, ast::ObjectType::FACTORY, "", NULL, NULL, NULL, NULL, func,
                 false, false);

    debug.functionEnd("PrintTypeObject");
  }

  template<typename Func>
  void SystemC::PrintFactory(parameters& parm, const std::string& name, Func func) {
    debug.functionStart("PrintFactory(name = " + name + ")", false,  __FILE__, __LINE__);
    std::string left;
    std::string right;
    std::string ascending;
    std::string instance_name = "factory_" + name;
    std::string factory_name = NameConverter::objectName(ast::ObjectType::FACTORY, name);
    auto f =
      [&](parameters& parm) {
	func(parm, left, right, ascending);
	bool arguments_exists = !left.empty();
	std::string arguments = arguments_exists ? left+ ", " + right : "";
	parm.addClassContents(name + " create() {", __FILE__, __LINE__);
	parm.addClassContents(name + " x(\"" + instance_name + "\");", __FILE__, __LINE__);
	parm.addClassContents("x.constrain(" + arguments + ");", __FILE__, __LINE__);
	parm.addClassContents("return x;", __FILE__, __LINE__);
	parm.addClassContents("}", __FILE__, __LINE__);
	parm.addClassContents("template <typename T>", __FILE__, __LINE__);
	parm.addClassContents(name + " create(T left, T right, bool ascending = true) {", __FILE__, __LINE__);
	parm.addClassContents(name + " x(left, right, ascending);", __FILE__, __LINE__);
	parm.addClassContents("return x;", __FILE__, __LINE__);
	parm.addClassContents("}", __FILE__, __LINE__);
      };
    PrintTypeObject(parm, name, f);
    parm.addClassBottom(factory_name + " " + instance_name + " = " + factory_name + "(this);");
    debug.functionEnd("PrintFactory");
  }


}

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
      m_debug.functionStart("ObjectDeclaration(type_name = " + type_name + ")", false, __FILE__, __LINE__);
      DatabaseResult database_result;
      if (parm.findOne(database_result, type_name, ast::ObjectType::TYPE)) { 
	m_debug.debug("database_result = " + database_result.toString(), __FILE__, __LINE__);
        type_name = NameConverter::getName(parm, database_result, false);
        std::string factory_arguments;
        if (v->type->range) {
          RangeDefinition range_definition;
          ast::ObjectValueContainer i = database_result.object->type;
          try {
            i = database_result.object->type.GetArgument();
          } catch (ast::ObjectValueContainer::IndexOutOfRange e) {}
          rangeToString(parm, v->type->range, range_definition, i);
          factory_arguments = range_definition.left + ", " + range_definition.right +
	    ", " + range_definition.ascending;
        }
        std::string factory_name = NameConverter::getName(parm, database_result, true, factory_arguments);
	ast::ObjectValueContainer type = database_result.object->type;
	if (type.GetTypeName().empty()) { 
	  type.setTypeName(type_name);
	}
        for (ast::SimpleIdentifier& id : v->identifiers.list) {
          std::string name = id.toString(true);
          m_debug.debug("Object name = " + name + ", type = " + type_name, __FILE__, __LINE__);
          if (database_enable) {parm.addObjectValueContainer(v->objectType, name, type, ast::ObjectInterface(), v->text);}
          std::string init = "";
          if (v->initialization) {
            auto sensitivity_list_callback = 
	      [&](DatabaseResult& object, std::string& name_extension) {};
	    ExpectedType expected_type(database_result.object->type);
	    init = a_expression.assignmentString(parm, v->initialization->value, expected_type,
						 sensitivity_list_callback, name);
            m_debug.debug("Init = " + init, __FILE__, __LINE__);
          }
          callback(name, type, type_name, init, factory_name, v->objectType, v->direction);
        }
      } else {
        exceptions.printError("Could not find type \"" + type_name + "\"", v->text);
      	if (a_verbose) {parm.printDatabase();}
      }
      m_debug.functionEnd("ObjectDeclaration");
    }
  }

  template<typename Func>
  void SystemC::PrintTypeObject(parameters& parm, const std::string& name, Func func) {
    m_debug.functionStart("PrintTypeObject(name = " + name + ")", false,  __FILE__, __LINE__);
    defineObject(parm, false, name, ast::ObjectType::FACTORY, "", NULL, NULL, NULL, NULL,
		 [&](parameters& parm){}, func, false, false);

    m_debug.functionEnd("PrintTypeObject");
  }

  template<typename Func>
  void SystemC::printFactoryDefinition(parameters& parm, const std::string& name,
				       Func func, const std::string subtype_name) {
    m_debug.functionStart("printFactoryDefinition(name = " + name +
			  ", subtype_name = " + subtype_name + ")", false,  __FILE__, __LINE__);
    RangeDefinition range_definition;
    RangeDefinition subtype_range_definition;
    std::string instance_name = NameConverter::getFactoryInstanceName(name);
    std::string factory_name = NameConverter::objectName(ast::ObjectType::FACTORY, name);
    auto f =
      [&](parameters& parm) {
	func(parm, range_definition, subtype_range_definition);
	bool arguments_exists = !range_definition.left.empty();
	bool subtype_arguments_exists = !subtype_range_definition.left.empty();
	std::string arguments = arguments_exists ? range_definition.left + ", " +
	  range_definition.right + NameConverter::listAppend(range_definition.ascending) : "";
	std::string subtype_arguments = subtype_arguments_exists ? subtype_range_definition.left + ", " +
	  subtype_range_definition.right + NameConverter::listAppend(subtype_range_definition.ascending) : "";
	parm.addClassContents(name + " create() {", __FILE__, __LINE__);
	if (subtype_arguments_exists) {
	  parm.addClassContents(subtype_name + " subtype(\"subtype\");", __FILE__, __LINE__);
	  parm.addClassContents("subtype.constrain(" + subtype_arguments + ");", __FILE__, __LINE__);
	}
	parm.addClassContents(name + " x(\"" + instance_name + "\");", __FILE__, __LINE__);
	std::string delimiter = (arguments_exists ? ", " : "");
	parm.addClassContents("x.constrain(" + arguments + (subtype_arguments_exists ? delimiter + "subtype" : "") +  ");", __FILE__, __LINE__);
	parm.addClassContents("return x;", __FILE__, __LINE__);
	parm.addClassContents("}", __FILE__, __LINE__);
	parm.addClassContents("template <typename T>", __FILE__, __LINE__);
	parm.addClassContents(name + " create(T left, T right, bool ascending = true) {", __FILE__, __LINE__);
	parm.addClassContents(name + " x(left, right, ascending);", __FILE__, __LINE__);
	parm.addClassContents("return x;", __FILE__, __LINE__);
	parm.addClassContents("}", __FILE__, __LINE__);
      };
    PrintTypeObject(parm, name, f);
    parm.addClassBottom("std::unique_ptr<" + factory_name + "> " + instance_name + ";");
    parm.addClassConstructorContents(instance_name + " = std::make_unique<" + factory_name + ">(this);", __FILE__, __LINE__);
    m_debug.functionEnd("printFactoryDefinition");
  }


}

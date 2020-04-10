#include <cassert>

#include "systemc.hpp"
#include "sequential.hpp"

namespace generator {

  void SystemC::instantiateType(parameters& parm, std::string name,
                                ast::ObjectType object_type,
				std::list<std::string>* sensitivity_list,
				std::string description_append,
				std::string instance_argument) {
    m_debug.functionStart("instantiateType");
    std::string object_name = NameConverter::objectName(object_type, name);
    std::string instance_name = object_name + "_INST";
    std::string options_name = "opts";
    bool spawn_method = !parm.process_contains_wait;
    parm.addClassConstructorContents("{", __FILE__, __LINE__);
    parm.addClassConstructorContents("sc_spawn_options " + options_name + ";", __FILE__, __LINE__);
    if (spawn_method) {
	parm.addClassConstructorContents(options_name + ".spawn_method();", __FILE__, __LINE__);
    }
    if (sensitivity_list) {
      for (auto& i : *sensitivity_list) {
	parm.addClassConstructorContents(options_name + ".set_sensitivity(" + i + ".getInterfacePointer());",
					 __FILE__, __LINE__);
      }
    }
    parm.addClassConstructorContents("std::string name = std::string(\"" + object_name + "\")" + description_append + ";",
				     __FILE__, __LINE__);
    parm.addClassBottom("std::unique_ptr<" + object_name + "> " + instance_name + ";");
    parm.addClassConstructorContents(instance_name + " = std::make_unique<" + object_name + ">(this" +
				     std::string(!instance_argument.empty() ? ", " + instance_argument : "") + ");",
				     __FILE__, __LINE__);
    parm.addClassConstructorContents("sc_spawn([&]() {", __FILE__, __LINE__);
    parm.addClassConstructorContents(instance_name + "->run();", __FILE__, __LINE__);
    parm.addClassConstructorContents("}, name.c_str(), &" + options_name +");", __FILE__, __LINE__);
    parm.addClassConstructorContents("}", __FILE__, __LINE__);
    m_debug.functionEnd("instantiateType");
  }

  void SystemC::componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l,
                                     std::string& entityName, std::string& library) {
    if (l) {
      auto func = [&](ast::AssociationElement a){
        std::string formalName = a.formalPart->name->toString(true);
        auto valid = [&] (DatabaseElement* e) { return true; };
        DatabaseResult object;
        if (parm.findOneBase(object, formalName, valid, entityName, library)) {
          ExpectedType formalType(object.object->type);
          return instanceName + "->" + formalName +
            "(" + a_expression.toString(parm, a.actualPart, formalType) + ".getInterfaceReference())";
        } else {
          exceptions.printError("Formal name " + formalName + " not found in entity " + library + "::" + entityName);
          parm.printAllObjects(formalName);
        }
        return std::string();
      };
      parm.addClassConstructorContents(listToString(parm, l->associationElements.list, "; ", func) + ";",
				       __FILE__, __LINE__);
    }
  }
  
  void SystemC::componentInstantiation(parameters& parm, ast::ComponentInstance* c) {
    if (c) {
      assert(c->instanceName);
      std::string instanceName = c->instanceName->toString(true);
      std::string componentName = c->componentName->toString(true);
      std::string architectureName = c->architectureName->toString(true);
      std::string s = componentName + "_" + architectureName;
      std::string libraryName;
      if (c->libraryName) {
        libraryName = c->libraryName->toString(true);
        if (libraryName != "WORK") {
          s = libraryName + "::" + s;
        }
      }
      std::string object_name = NameConverter::objectName(ast::ObjectType::ARCHITECTURE, s);
      parm.addClassConstructorContents(getSourceLine(c->instanceName->text), __FILE__, __LINE__);
      parm.addClassConstructorContents("auto " + instanceName + " = new " + object_name + "(\"" + instanceName + "\");",
				       __FILE__, __LINE__);
      componentAssociation(parm, instanceName, c->generics, componentName, libraryName);
      componentAssociation(parm, instanceName, c->ports, componentName, libraryName);
    }
  }

  /* TODO: Move to concurrentStatementsDefinition */ 
  void SystemC::concurrentStatementsInstantiation(parameters& parm,
                                                 ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    m_debug.functionStart("concurrentStatementsInstantiation");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      componentInstantiation(parm, c.componentInstance);
    }
    m_debug.functionEnd("concurrentStatementsInstantiation");
  }

  std::string SystemC::ObjectName(const ParentInfo& info) {
    ast::ObjectType type = (info.type == ast::ObjectType::PACKAGE_BODY ? ast::ObjectType::PACKAGE : info.type);
    return NameConverter::objectName(type, info.name);
  }
  
  void SystemC::createConstructor(parameters& parm, bool topHierarchy, ast::ObjectType type,
                                  std::string& name, std::string* argument,
                                  ast::List<ast::ConcurrentStatement>* concurrentStatements) {
    if (!topHierarchy) {
      ParentInfo parent_info;
      parm.getParent(parent_info);
      if (parent_info.name.size() > 0) {
	std::string constructor_description = ObjectName(parent_info) +"* parent";
	parm.addClassConstructorInitializer("p(parent)");
	if (argument) {
	  constructor_description += ", auto " + *argument;
	  parm.addClassConstructorInitializer(*argument + "(" + *argument + ")");
	}
	parm.setClassConstructorDescription(NameConverter::objectName(type, name) + "(" + constructor_description + ")");
      }
    }
  }

}

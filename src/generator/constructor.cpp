#include <cassert>

#include "systemc.hpp"
#include "sequential.hpp"

namespace generator {

  void SystemC::instantiateType(parameters& parm, std::string name,
                                ast::ObjectType object_type,
				std::list<std::string>* sensitivity_list) {
    debug.functionStart("instantiateType");
    std::string n = NameConverter::objectName(object_type, name);
    std::string i = n + "_INST";
    std::string o = n + "_OPTIONS";
    parm.addClassBottom("std::unique_ptr<" + n + "> " + i + ";");
    bool spawn_method = !parm.process_contains_wait;
    std::string sensitivity_exists_argument = std::string((sensitivity_list) && !sensitivity_list->empty() ? "true" : "false");
    std::string spawn_method_argument = std::string(spawn_method ? "true" : "false");
    parm.addClassConstructorContents(i + " = std::make_unique<" + n + ">(this, " + sensitivity_exists_argument + ", " + spawn_method_argument + ");");
    parm.addClassConstructorContents("sc_spawn_options " + o + ";");
    if (spawn_method) {
	parm.addClassConstructorContents(o + ".spawn_method();");
    }
    if (sensitivity_list) {
      for (auto& i : *sensitivity_list) {
	parm.addClassConstructorContents(o + ".set_sensitivity(" + i + ".getInterfacePointer());");
      }
    }
    parm.addClassConstructorContents("sc_spawn([&]() {" + i + "->run();}, \"" + n + "\", &" + o +");");
    debug.functionEnd("instantiateType");
  }

  void SystemC::blockStatementInstantiation(parameters& parm,
                                            ast::BlockStatement* blockStatement) {
    if (blockStatement) {
      debug.functionStart("blockStatementInstantiation");
      instantiateType(parm, blockStatement->name->toString(true), ast::ObjectType::BLOCK);
      debug.functionEnd("blockStatementInstantiation");
    }
  }

  void SystemC::forGenerateStatementInstantiation(parameters& parm,
                                                  ast::ForGenerateStatement* forGenerateStatement) {
    
    if (forGenerateStatement) {
      debug.functionStart("forGenerateStatementInstantiation");
      assert(forGenerateStatement->identifier);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      std::string name = forGenerateStatement->name->toString(true);
      parm.addObject(ast::ObjectType::VARIABLE, identifier, ast::ObjectValue::INTEGER);
      forLoop(parm, identifier, forGenerateStatement->iteration, [&](parameters& parm) {
          instantiateType(parm, name, ast::ObjectType::GENERATE);
        }, true);
      debug.functionEnd("forGenerateStatementInstantiation");
    }
  }

  void SystemC::componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l,
                                     std::string& entityName, std::string& library) {
    if (l) {
      auto func = [&](ast::AssociationElement a){
        std::string formalName = a.formalPart->name->toString(true);
        auto valid = [&] (DatabaseElement* e) { return true; };
        DatabaseResult object;
        if (parm.findOneBase(object, formalName, valid, entityName, library)) {
          ast::ObjectValueContainer formalType = object.object->type;
          return instanceName + "->" + formalName +
            "(" + a_expression.toString(parm, a.actualPart, formalType) + ".getInterfaceReference())";
        } else {
          exceptions.printError("Formal name " + formalName + " not found in entity " + library + "::" + entityName);
          parm.printAllObjects(formalName);
        }
        return std::string();
      };
      parm.addClassConstructorContents(listToString(parm, l->associationElements.list, "; ", func) + ";");
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
      parm.addClassConstructorContents(getSourceLine(c->instanceName->text));
      parm.addClassConstructorContents("auto " + instanceName + " = new " + object_name + "(\"" + instanceName + "\");");
      componentAssociation(parm, instanceName, c->generics, componentName, libraryName);
      componentAssociation(parm, instanceName, c->ports, componentName, libraryName);
    }
  }

  /* TODO: Move to concurrentStatementsDefinition */ 
  void SystemC::concurrentStatementsInstantiation(parameters& parm,
                                                 ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    debug.functionStart("concurrentStatementsInstantiation");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      forGenerateStatementInstantiation(parm, c.forGenerateStatement);
      blockStatementInstantiation(parm, c.blockStatement);



      componentInstantiation(parm, c.componentInstance);
    }
    debug.functionEnd("concurrentStatementsInstantiation");
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
	std::string constructor_description = ObjectName(parent_info) +"* parent, bool static_sensitivity_exists = false, bool spawn_method = false";
	parm.addClassConstructorInitializer("p(parent)");
	parm.addClassContents("bool m_static_sensitivity_exists;");
	parm.addClassContents("bool m_spawn_method;");
	parm.addClassConstructorContents("m_static_sensitivity_exists = static_sensitivity_exists;");
	parm.addClassConstructorContents("m_spawn_method = spawn_method;");
	if (argument) {
	  constructor_description += ", auto " + *argument;
	  parm.addClassConstructorInitializer(*argument + "(" + *argument + ")");
	}
	parm.setClassConstructorDescription(NameConverter::objectName(type, name) + "(" + constructor_description + ")");
      }
    }
  }

}

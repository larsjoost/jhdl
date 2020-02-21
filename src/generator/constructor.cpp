#include <cassert>

#include "systemc.hpp"
#include "sequential.hpp"

namespace generator {

  void SystemC::instantiateType(parameters& parm, std::string name,
                                ast::ObjectType object_type, std::string arguments) {
    debug.functionStart("instantiateType");
    std::string n = ObjectName(object_type, name);
    std::string i = n + "_INST";
    parm.addClassBottom("std::unique_ptr<" + n + "> " + i + ";");
    parm.addClassConstructorContents(i + " = std::make_unique<" + n + ">(this);");
    parm.addClassConstructorContents("sc_spawn([&]() {" + i + "->run();}, \"" + n + "\");");
    debug.functionEnd("instantiateType");
  }

  void SystemC::signalInstantiation(parameters& parm, ast::SignalAssignment* s) {
    if (s) {
      debug.functionStart("signalInstantiation");
      instantiateType(parm, s->name, ast::ObjectType::PROCESS);
      debug.functionEnd("signalInstantiation");
    }
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
          instantiateType(parm, name, ast::ObjectType::GENERATE, ", " + identifier);
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
            ".bind(" + a_expression.toString(parm, a.actualPart, formalType) + ")";
        } else {
          exceptions.printError("Formal name " + formalName + " not found in entity " + library + "::" + entityName);
          parm.printAllObjects(formalName);
        }
        return std::string();
      };
      parm.addClassContents(listToString(parm, l->associationElements.list, "; ", func) + ";");
    }
  }
  
  void SystemC::componentInstantiation(parameters& parm, ast::ComponentInstance* c) {
    if (c) {
      assert(c->instanceName);
      std::string instanceName = c->instanceName->toString(true);
      std::string componentName = c->componentName->toString(true);
      std::string s = componentName;
      std::string libraryName;
      if (c->libraryName) {
        libraryName = c->libraryName->toString(true);
        if (libraryName != "WORK") {
          s = libraryName + "::" + s;
        }
      }
      std::string object_name = ObjectName(ast::ObjectType::ARCHITECTURE, s);
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
      signalInstantiation(parm, c.signalAssignment);
      componentInstantiation(parm, c.componentInstance);
    }
  }

  std::string SystemC::ObjectName(const ParentInfo& info) {
    ast::ObjectType type = (info.type == ast::ObjectType::PACKAGE_BODY ? ast::ObjectType::PACKAGE : info.type);
    return ObjectName(type, info.name);
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
	parm.setClassConstructorDescription(ObjectName(type, name) + "(" + constructor_description + ")");
      }
    }
  }

}

#include <cassert>

#include "systemc.hpp"
#include "sequential.hpp"

namespace generator {

  void SystemC::instantiateType(parameters& parm, std::string type, std::string name,
                                std::string arguments) {
    debug.functionStart("instantiateType");
    parm.println(parameters::Area::CONSTRUCTOR, type + "(new " + name + "(this" + arguments + "));");
    debug.functionEnd("instantiateType");
  }

  void SystemC::methodInstantiation(parameters& parm, ast::Method* method) {
    if (method) {
      debug.functionStart("methodInstantiation");
      std::string methodName;
      if (method->label) {
        methodName = method->label->toString(true);
      } else {
        methodName = method->noname;
      }
      instantiateType(parm, "SC_NEW_THREAD", methodName);
      debug.functionEnd("methodInstantiation");
    }
  }

  void SystemC::signalInstantiation(parameters& parm, ast::SignalAssignment* s) {
    if (s) {
      debug.functionStart("signalInstantiation");
      instantiateType(parm, "SC_NEW_THREAD", s->name);
      debug.functionEnd("signalInstantiation");
    }
  }

  void SystemC::blockStatementInstantiation(parameters& parm,
                                            ast::BlockStatement* blockStatement) {
    if (blockStatement) {
      debug.functionStart("blockStatementInstantiation");
      instantiateType(parm, "SC_NEW_BLOCK", blockStatement->name->toString(true));
      debug.functionEnd("blockStatementInstantiation");
    }
  }

  void SystemC::forGenerateStatementInstantiation(parameters& parm,
                                                  ast::ForGenerateStatement* forGenerateStatement) {
    
    if (forGenerateStatement) {
      assert(forGenerateStatement->identifier);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      std::string name = forGenerateStatement->name->toString(true);
      a_database.add(ast::ObjectType::VARIABLE, identifier, ast::ObjectValue::INTEGER);
      forLoop(parm, identifier, forGenerateStatement->iteration, [&](parameters& parm) {
          instantiateType(parm, "SC_NEW_FOR_GENERATE", name, ", " + identifier);
        });
    }
  }

  void SystemC::componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l,
                                     std::string& entityName, std::string& library) {
    if (l) {
      auto func = [&](ast::AssociationElement a){
        std::string formalName = a.formalPart->name->toString(true);
        auto valid = [&] (DatabaseElement* e) { return true; };
        DatabaseResult object;
        if (a_database.findOne(object, formalName, valid, entityName, library)) {
          ast::ObjectValueContainer formalType = object.object->type;
          return instanceName + "->" + formalName +
            ".bind(" + a_expression.toString(a.actualPart, formalType) + ")";
        } else {
          exceptions.printError("Formal name " + formalName + " not found in entity " + library + "::" + entityName);
          a_database.printAllObjects(formalName);
        }
        return std::string();
      };
      parm.println(listToString(parm, l->associationElements.list, "; ", func) + ";");
    }
  }
  
  void SystemC::componentInstantiation(parameters& parm, ast::ComponentInstance* c) {
    if (c) {
      assert(c->instanceName);
      printSourceLine(parm, c->instanceName->text);
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
      parm.println(parameters::Area::CONSTRUCTOR, "auto " + instanceName + " = new " + s + "(\"" + instanceName + "\");");
      componentAssociation(parm, instanceName, c->generics, componentName, libraryName);
      componentAssociation(parm, instanceName, c->ports, componentName, libraryName);
    }
  }
  
  void SystemC::concurrentStatementsInstantiation(parameters& parm,
                                                 ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    debug.functionStart("concurrentStatementsInstantiation");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodInstantiation(parm, c.method);
      forGenerateStatementInstantiation(parm, c.forGenerateStatement);
      blockStatementInstantiation(parm, c.blockStatement);
      signalInstantiation(parm, c.signalAssignment);
      componentInstantiation(parm, c.componentInstance);
    }
  }

  std::string SystemC::getConstructorDeclaration(parameters& parm, std::string& type, std::string& name, std::string* argument) {
    std::string parentName = a_database.getParentName();
    if (parentName.size() > 0) {
      std::string p1 = parentName +"* parent";
      std::string p2 = " : p(parent)";
      if (argument) {
        p1 += ", auto " + *argument;
        p2 += ", " + *argument + "(" + *argument + ")";
      }
      return type + "_CTOR(" + name + ")" + "(" + p1 + ") " + p2;
    }
    return "SC_CTOR(" + name + ")";
  }

  void SystemC::createConstructor(parameters& parm, bool topHierarchy, std::string& type,
                                  std::string& name, std::string* argument,
                                  ast::List<ast::ConcurrentStatement>* concurrentStatements) {
    parm.println("void init() {");
    if (concurrentStatements) {
      parm.incIndent();
      concurrentStatementsInstantiation(parm, *concurrentStatements);
      parm.decIndent();
    }
    parm.Flush(parameters::Area::CONSTRUCTOR);
    parm.println("}");
    if (!topHierarchy) {
      parm.println(getConstructorDeclaration(parm, type, name, argument) + " {");
      parm.incIndent();
      parm.println("init();");
      parm.decIndent();
      parm.println("}");
    }
  }

}

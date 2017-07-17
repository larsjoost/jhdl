#include <cassert>

#include "systemc.hpp"
#include "sequential.hpp"

namespace generator {

  void SystemC::instantiateType(parameters& parm, std::string type, std::string name,
                                std::string arguments) {
    functionStart("instantiateType");
    parm.println(type + "(new " + name + "(this" + arguments + "));");
  }

  void SystemC::methodInstantiation(parameters& parm, ast::Method* method) {
    functionStart("methodInstantiation");
    if (method) {
      std::string methodName;
      if (method->label) {
        methodName = method->label->toString(true);
      } else {
        methodName = method->noname;
      }
      instantiateType(parm, "SC_NEW_THREAD", methodName);
    }
    functionEnd("methodInstantiation");
  }

  void SystemC::signalInstantiation(parameters& parm, ast::SignalAssignment* s) {
    functionStart("signalInstantiation");
    if (s) {
      instantiateType(parm, "SC_NEW_THREAD", s->name);
    }
    functionEnd("signalInstantiation");
  }

  void SystemC::blockStatementInstantiation(parameters& parm,
                                            ast::BlockStatement* blockStatement) {
    functionStart("blockStatementInstantiation");
    if (blockStatement) {
      instantiateType(parm, "SC_NEW_BLOCK", blockStatement->name->toString(true));
    }
  }

  void SystemC::forGenerateStatementInstantiation(parameters& parm,
                                                  ast::ForGenerateStatement* forGenerateStatement) {
    
    if (forGenerateStatement) {
      assert(forGenerateStatement->identifier);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      std::string name = forGenerateStatement->name->toString(true);
      database.add(ast::VARIABLE, identifier, ast::INTEGER);
      forLoop(parm, identifier, forGenerateStatement->range, [&](parameters& parm) {
          instantiateType(parm, "SC_NEW_FOR_GENERATE", name, ", " + identifier);
        });
    }
  }

  void SystemC::componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l) {
    if (l) {
      // TODO: resolve ast::UNKNOWN
      auto func = [&](ast::AssociationElement a){
        ExpressionParser expr(&database);
        return instanceName + "->" + a.formalPart->name->toString(true) +
        ".bind(" + expr.toString(a.actualPart, ast::UNKNOWN) + ")";
      };
      parm.println(listToString(parm, l->associationElements.list, "; ", func) + ";");
    }
  }
  
  void SystemC::componentInstantiation(parameters& parm, ast::ComponentInstance* c) {
    if (c) {
      assert(c->instanceName);
      printSourceLine(parm, c->instanceName->text);
      std::string instanceName = c->instanceName->toString(true);
      std::string componentName = "";
      std::string delimiter = "";
      for (ast::SimpleIdentifier i : c->componentName->list) {
        std::string n = i.toString(true);
        if (n != "WORK") {
          componentName += delimiter + n;
          delimiter = "::";
        }
      }
      parm.println("auto " + instanceName + " = new " + componentName + "(\"" + instanceName + "\");");
      componentAssociation(parm, instanceName, c->generics);
      componentAssociation(parm, instanceName, c->ports);
    }
  }
  
  void SystemC::concurrentStatementsInstantiation(parameters& parm,
                                                 ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    functionStart("concurrentStatementsInstantiation");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodInstantiation(parm, c.method);
      forGenerateStatementInstantiation(parm, c.forGenerateStatement);
      blockStatementInstantiation(parm, c.blockStatement);
      signalInstantiation(parm, c.signalAssignment);
      componentInstantiation(parm, c.componentInstance);
    }
  }

  std::string SystemC::getConstructorDeclaration(parameters& parm, std::string& name, std::string* argument) {
    std::string parentName = database.getParentName();
    if (parentName.size() > 0) {
      std::string p1 = parentName +"* parent";
      std::string p2 = " : p(parent)";
      if (argument) {
        p1 += ", auto " + *argument;
        p2 += ", " + *argument + "(" + *argument + ")";
      }
      return name + "(" + p1 + ") " + p2;
    }
    return "SC_CTOR(" + name + ")";
  }

  void SystemC::createConstructor(parameters& parm, std::string& name,
                                  std::string* argument,
                                  ast::List<ast::ConcurrentStatement>* concurrentStatements) {
    parm.println(getConstructorDeclaration(parm, name, argument) + " {");
    if (concurrentStatements) {
      parm.incIndent();
      concurrentStatementsInstantiation(parm, *concurrentStatements);
      parm.decIndent();
    }
    parm.println("}");
  }

}

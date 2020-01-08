#include "systemc.hpp"
#include "definition.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {


  void SystemC::blockStatementDefinition(parameters& parm,
                                         ast::BlockStatement* blockStatement) {
    debug.functionStart("blockStatementDefinition");
    if (blockStatement) {
      std::string name = blockStatement->name->toString(true);
      std::string class_description = "struct " + name;
      defineObject(parm, false, name, ast::ObjectType::BLOCK,
		   class_description, NULL, 
                   &blockStatement->declarations,
                   &blockStatement->concurrentStatements,
                   [&](parameters& parm){},
		   [&](parameters& parm){},
		   false, true);
    }
  }

  void SystemC::forGenerateStatementDefinition(parameters& parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      parm.addImplementationContents(getSourceLine(forGenerateStatement->name));
      std::string name = forGenerateStatement->name->toString(true);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      auto createDeclaration = [&](parameters& parm) {
        parm.addObject(ast::ObjectType::VARIABLE, identifier, ast::ObjectValue::INTEGER);
      };
      auto createBody = [&](parameters& parm) {
      	parm.addImplementationContents("STD::STANDARD::INTEGER " + identifier + ";");
      };
      std::string class_description = "struct " + name;
      defineObject(parm, false, name, ast::ObjectType::GENERATE,
		   class_description, &identifier,
                   &forGenerateStatement->declarations,
                   &forGenerateStatement->concurrentStatements,
		   createBody, createDeclaration, false, true);
    }
  }

  template <typename Func>
  void SystemC::createProcess(parameters& parm, Func func, std::string name) {
    parm.addClassConstructorContents("SC_METHOD(" + name + ")");
    parm.addClassContents("void " + name + "() {");
    func(parm);
    parm.addClassContents("}");
  }
  
  void SystemC::methodDefinition(parameters& parm, ast::Method* method) {
    debug.functionStart("methodDefinition");
    if (method) {
      std::string methodName;
      if (method->label) {
        methodName = method->label->toString(true);
      } else {
        methodName = "noname" + std::to_string(methodId++);
        method->noname = methodName;
      }
      auto createDefinition = [&](parameters& parm) {
      };
      auto createBody = [&](parameters& parm) {
        createProcess(parm,
                      [&](parameters& parm) {},
		      methodName);
      };
      std::string class_description = "struct " + methodName;
      defineObject(parm, false, methodName, ast::ObjectType::PROCESS,
		   class_description, NULL,
                   &method->declarations, NULL, createBody, createDefinition, true, true);
    }
    debug.functionEnd("methodDefinition");
  }

  void SystemC::concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s) {
    if (s) {
      debug.functionStart("concurrentSignalAssignment");
      std::string name;
      if (s->label) {
        name = s->label->toString(true);
      } else {
        name = "line" + std::to_string(s->target->getText().getLine());
      }
      s->name = name;
      {
        bool q = parm.setQuiet(true);
        std::list<std::string> sensitivity;
        auto sensitivityListGenerator =
	  [&](DatabaseResult& object) {
	    sensitivity.push_back(parm.getName(object));
	  };
        signalAssignment(parm, s, sensitivityListGenerator);
        parm.setQuiet(q);
        auto func = [&](std::string& s) {
          std::string name = s;
          getObjectName(parm, name, ast::ObjectType::SIGNAL);
          return name;
        };
        auto createBody = [&](parameters& parm) {
          createProcess(parm,
                        [&](parameters& parm) {
                          printSensitivityListWait(parm, sensitivity, func);
                          signalAssignment(parm, s);
                        },
			name);
        };
        std::string class_description = "struct " + name;
	defineObject(parm, false, name, ast::ObjectType::PROCESS,
		     class_description, NULL, NULL, NULL, createBody,
		     [&](parameters& parm) {}, true, true);
      }
      debug.functionEnd("concurrentSignalAssignment");
    }
  }
  
  void SystemC::concurrentStatementsDefinition(parameters& parm,
                                               ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    debug.functionStart("concurrentStatementsDefinition");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodDefinition(parm, c.method);
      forGenerateStatementDefinition(parm, c.forGenerateStatement);
      blockStatementDefinition(parm, c.blockStatement);
      concurrentSignalAssignment(parm, c.signalAssignment);
    }
    debug.functionEnd("concurrentStatementsDefinition");
  }


}

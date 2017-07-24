#include "systemc.hpp"
#include "definition.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {


  void SystemC::blockStatementDefinition(parameters& parm,
                                         ast::BlockStatement* blockStatement) {
    functionStart("blockStatementDefinition");
    if (blockStatement) {
      printSourceLine(parm, blockStatement->name);
      std::string name = blockStatement->name->toString(true);
      defineObject(parm, name, "SC_BLOCK", NULL, NULL,
                   &blockStatement->declarations,
                   &blockStatement->concurrentStatements,
                   [&](parameters& parm){},
		   [&](parameters& parm){}
		   );
    }
  }

  void SystemC::forGenerateStatementDefinition(parameters& parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      printSourceLine(parm, forGenerateStatement->name);
      std::string name = forGenerateStatement->name->toString(true);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      database.add(ast::VARIABLE, identifier, ast::INTEGER);
      auto createDeclaration = [&](parameters& parm) {};
      auto createBody = [&](parameters& parm) {
	parm.println("INTEGER<> " + identifier + ";");
      };
      defineObject(parm,
                   name,
                   "SC_FOR_GENERATE",
		   NULL,
		   &identifier,
                   &forGenerateStatement->declarations,
                   &forGenerateStatement->concurrentStatements,
		   createBody,
		   createDeclaration
                   );
    }
  }

  template <typename Func>
  void SystemC::createProcess(parameters& parm, Func func) {
    parm.println("void process() {");
    parm.incIndent();
    func(parm);
    parm.decIndent();
    parm.println("}");
  }
  
  void SystemC::methodDefinition(parameters& parm, ast::Method* method) {
    functionStart("methodDefinition");
    if (method) {
      std::string methodName;
      if (method->label) {
        methodName = method->label->toString(true);
      } else {
        methodName = "noname" + std::to_string(methodId++);
        method->noname = methodName;
      }
      auto createDefinition = [&](parameters& parm) {
	parm.area = parameters::DECLARATION;
	sequentialStatements(parm, method->sequentialStatements);
      };
      auto createBody = [&](parameters& parm) {
        createProcess(parm,
                      [&](parameters& parm) {
                        if (method->sensitivity) {
                          auto s = [&](ast::SimpleIdentifier& s) {
                            return s.toString(true);
                          };
                          printSensitivityListWait(parm, method->sensitivity, s);
                        }
			parm.println("// Wait statements goto tree");
			parm.area = parameters::INITIALIZATION;
			parm.index = 1;
			parm.println("switch (waitIndex) {");
			parm.incIndent();
                        sequentialStatements(parm, method->sequentialStatements);
			parm.println("default: waitIndex = 1;");
			parm.decIndent();
			parm.println("}");
			parm.println("// Implementation area");
			parm.area = parameters::IMPLEMENTATION;
                        sequentialStatements(parm, method->sequentialStatements);
                      });
      };
      defineObject(parm, methodName, "SC_THREAD", NULL, NULL,
                   &method->declarations, NULL, createBody, createDefinition);
    }
    functionEnd("methodDefinition");
  }

  void SystemC::concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s) {
    
    if (s) {
      functionStart("concurrentSignalAssignment");
      std::string name;
      if (s->label) {
        name = s->label->toString(true);
      } else {
        name = "line" + std::to_string(s->identifier->text.getLine());
      }
      s->name = name;
      
      {
        std::list<std::string> sensitivity;
        quiet = true;
        signalAssignment(parm, s, [&](DatabaseResult& object) {
            sensitivity.push_back(object.getName(false));
          });
        quiet = false;
        auto func = [&](std::string& s) { return s; };
        auto createBody = [&](parameters& parm) {
          createProcess(parm,
                        [&](parameters& parm) {
                          printSensitivityListWait(parm, sensitivity, func);
                          signalAssignment(parm, s);
                        });
        };
        defineObject(parm, name, "SC_THREAD", NULL, NULL, NULL, NULL, createBody,
		     [&](parameters& parm) {});
      }
      functionEnd("concurrentSignalAssignment");
    }
  }
  
  void SystemC::concurrentStatementsDefinition(parameters& parm,
                                               ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    functionStart("concurrentStatementsDefinition");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodDefinition(parm, c.method);
      forGenerateStatementDefinition(parm, c.forGenerateStatement);
      blockStatementDefinition(parm, c.blockStatement);
      concurrentSignalAssignment(parm, c.signalAssignment);
    }
    functionEnd("concurrentStatementsDefinition");
  }


}

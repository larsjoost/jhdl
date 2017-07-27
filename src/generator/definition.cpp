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
      defineObject(parm, name, "SC_BLOCK", NULL, 
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
                          auto s = [&](ast::SimpleIdentifier& name) {
                            std::string x = name.toString(true);
                            getObjectName(x, ast::SIGNAL);
                            return x;
                          };
                          printSensitivityListWait(parm, method->sensitivity, s);
                        }
                        parm.println("// Wait statements goto tree");
			parm.setArea(parameters::INITIALIZATION);
			parm.printArea("Process");
			parm.println("switch (w.index) {");
			parm.incIndent();
                        sequentialStatements(parm, method->sequentialStatements);
			parm.decIndent();
			parm.println("}");
			parm.setArea(parameters::IMPLEMENTATION);
			parm.printArea("Process");
			sequentialStatements(parm, method->sequentialStatements);
                      });
      };
      defineObject(parm, methodName, "SC_THREAD", NULL,
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
        bool q = parm.setQuiet(true);
        std::list<std::string> sensitivity;
        auto sensitivityListGenerator = [&](DatabaseResult& object) {
          sensitivity.push_back(object.getName(false, database.getHierarchyLevel()));
        };
        signalAssignment(parm, s, sensitivityListGenerator);
        parm.setQuiet(q);
        auto func = [&](std::string& s) {
          std::string name = s;
          getObjectName(name, ast::SIGNAL);
          return name;
        };
        auto createBody = [&](parameters& parm) {
          createProcess(parm,
                        [&](parameters& parm) {
                          printSensitivityListWait(parm, sensitivity, func);
                          parm.setArea(parameters::IMPLEMENTATION);
                          parm.printArea("Concurrent signal assignment");
                          signalAssignment(parm, s);
                        });
        };
        defineObject(parm, name, "SC_THREAD", NULL, NULL, NULL, createBody,
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

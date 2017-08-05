#include "systemc.hpp"
#include "definition.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {


  void SystemC::blockStatementDefinition(parameters& parm,
                                         ast::BlockStatement* blockStatement) {
    debug.functionStart("blockStatementDefinition");
    if (blockStatement) {
      printSourceLine(parm, blockStatement->name);
      std::string name = blockStatement->name->toString(true);
      defineObject(parm, false, name, "SC_BLOCK", NULL, 
                   &blockStatement->declarations,
                   &blockStatement->concurrentStatements,
                   [&](parameters& parm){},
		   [&](parameters& parm){},
		   false);
    }
  }

  void SystemC::forGenerateStatementDefinition(parameters& parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      printSourceLine(parm, forGenerateStatement->name);
      std::string name = forGenerateStatement->name->toString(true);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      auto createDeclaration = [&](parameters& parm) {
        a_database.add(ast::ObjectType::VARIABLE, identifier, ast::ObjectValue::INTEGER);
      };
      auto createBody = [&](parameters& parm) {
      	parm.println("STD::STANDARD::INTEGER<> " + identifier + ";");
      };
      defineObject(parm, false, name, "SC_FOR_GENERATE", &identifier,
                   &forGenerateStatement->declarations,
                   &forGenerateStatement->concurrentStatements,
		   createBody, createDeclaration, false);
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
	parm.area = parameters::Area::DECLARATION;
	sequentialStatements(parm, method->sequentialStatements);
      };
      auto createBody = [&](parameters& parm) {
        createProcess(parm,
                      [&](parameters& parm) {
                        parm.println("// Wait statements goto tree");
			parm.setArea(parameters::Area::INITIALIZATION);
			parm.printArea("Process");
			parm.println("switch (w.index) {");
			parm.incIndent();
                        sequentialStatements(parm, method->sequentialStatements);
			parm.decIndent();
			parm.println("}");
			parm.setArea(parameters::Area::IMPLEMENTATION);
			if (method->sensitivity) {
                          auto s = [&](ast::SimpleIdentifier& name) {
                            std::string x = name.toString(true);
                            getObjectName(x, ast::ObjectType::SIGNAL);
                            return x;
                          };
                          printSensitivityListWait(parm, method->sensitivity, s);
                        }
                        parm.printArea("Process");
			sequentialStatements(parm, method->sequentialStatements);
                      });
      };
      defineObject(parm, false, methodName, "SC_THREAD", NULL,
                   &method->declarations, NULL, createBody, createDefinition, true);
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
        name = "line" + std::to_string(s->identifier->text.getLine());
      }
      s->name = name;
      
      {
        bool q = parm.setQuiet(true);
        std::list<std::string> sensitivity;
        auto sensitivityListGenerator = [&](DatabaseResult& object) {
          sensitivity.push_back(a_name_converter.getName(object, false));
        };
        signalAssignment(parm, s, sensitivityListGenerator);
        parm.setQuiet(q);
        auto func = [&](std::string& s) {
          std::string name = s;
          getObjectName(name, ast::ObjectType::SIGNAL);
          return name;
        };
        auto createBody = [&](parameters& parm) {
          createProcess(parm,
                        [&](parameters& parm) {
                          printSensitivityListWait(parm, sensitivity, func);
                          parm.setArea(parameters::Area::IMPLEMENTATION);
                          parm.printArea("Concurrent signal assignment");
                          signalAssignment(parm, s);
                        });
        };
        defineObject(parm, false, name, "SC_THREAD", NULL, NULL, NULL, createBody,
		     [&](parameters& parm) {}, true);
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

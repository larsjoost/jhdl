#include "systemc.hpp"
#include "definition.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {


  void SystemC::blockStatementDefinition(parameters& parm,
                                         ast::BlockStatement* blockStatement) {
    debug.functionStart("blockStatementDefinition");
    if (blockStatement) {
      PrintSourceLine(parm, blockStatement->name);
      std::string name = blockStatement->name->toString(true);
      DefineObject(parm, false, name, ast::ObjectType::BLOCK, "sc_module", NULL, 
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
      PrintSourceLine(parm, forGenerateStatement->name);
      std::string name = forGenerateStatement->name->toString(true);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      auto createDeclaration = [&](parameters& parm) {
        a_database.add(ast::ObjectType::VARIABLE, identifier, ast::ObjectValue::INTEGER);
      };
      auto createBody = [&](parameters& parm) {
      	parm.println("STD::STANDARD::INTEGER " + identifier + ";");
      };
      DefineObject(parm, false, name, ast::ObjectType::GENERATE, "sc_module", &identifier,
                   &forGenerateStatement->declarations,
                   &forGenerateStatement->concurrentStatements,
		   createBody, createDeclaration, false, true);
    }
  }

  template <typename Func>
  void SystemC::createProcess(parameters& parm, Func func) {
    parm.println("void process() {");
    func(parm);
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
      };
      auto createBody = [&](parameters& parm) {
        createProcess(parm,
                      [&](parameters& parm) {
			parm.SetArea(parameters::Area::INITIALIZATION);
                        parm.println("// Wait statements goto tree");
			parm.println("switch (w.index) {");
                        parm.index = 1;
                        sequentialStatements(parm, method->sequentialStatements);
			parm.println("}");
                        if (method->sensitivity) {
                          auto s = [&](ast::SimpleIdentifier& name) {
                            std::string x = name.toString(true);
                            getObjectName(x, ast::ObjectType::SIGNAL);
                            return x;
                          };
                          printSensitivityListWait(parm, method->sensitivity, s);
                        }
                        parm.SetArea(parameters::Area::IMPLEMENTATION);
                      });
      };
      parm.println("");
      DefineObject(parm, false, methodName, ast::ObjectType::PROCESS, "sc_thread", NULL,
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
        name = "line" + std::to_string(s->target->identifier->text.getLine());
      }
      s->name = name;
      {
        bool q = parm.setQuiet(true);
        std::list<std::string> sensitivity;
        auto sensitivityListGenerator = [&](DatabaseResult& object) {
          sensitivity.push_back(a_name_converter.GetName(object));
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
                          signalAssignment(parm, s);
                        });
        };
        parm.println("");
        DefineObject(parm, false, name, ast::ObjectType::PROCESS, "sc_thread", NULL, NULL, NULL, createBody,
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

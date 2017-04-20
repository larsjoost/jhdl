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
      defineObject(parm,
                   name,
                   "SC_BLOCK",
                   NULL,
                   &blockStatement->declarations,
                   &blockStatement->concurrentStatements,
                   [&](parameters& parm){});
    }
  }

  void SystemC::forGenerateStatementDefinition(parameters& parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      printSourceLine(parm, forGenerateStatement->name);
      std::string name = forGenerateStatement->name->toString(true);
      std::string identifier = forGenerateStatement->identifier->toString(true);
      database.add(ast::VARIABLE, identifier, ast::INTEGER);
      defineObject(parm,
                   name,
                   "SC_FOR_GENERATE",
                   &identifier,
                   &forGenerateStatement->declarations,
                   &forGenerateStatement->concurrentStatements,
                   [&](parameters& parm){
                     println(parm, "INTEGER<> " + identifier + ";");
                   });
    }
  }

  template <typename Func>
  void SystemC::createProcess(parameters& parm, Func func) {
    println(parm, "void process() {");
    parm.incIndent();
    func(parm);
    parm.decIndent();
    println(parm, "}");
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
      auto createBody = [&](parameters& parm) {
        createProcess(parm,
                      [&](parameters& parm) {
                        if (method->sensitivity) {
                          auto s = [&](ast::SimpleIdentifier& s) {
                            return s.toString(true);
                          };
                          println(parm, createWait(parm, method->sensitivity, s));
                        }
                        sequentialStatements(parm, method->sequentialStatements);
                      });
      };
      defineObject(parm, methodName, "SC_THREAD", NULL,
                   &method->declarations,
                   NULL, createBody);
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
                          println(parm, createWait(parm, sensitivity, func));
                          signalAssignment(parm, s);
                        });
        };
        defineObject(parm, name, "SC_THREAD", NULL, NULL, NULL, createBody);
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

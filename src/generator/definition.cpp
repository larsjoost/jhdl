#include "systemc.hpp"
#include "definition.hpp"
#include "sequential.hpp"
#include "expression.hpp"

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
      addDeclarationType(parm, forGenerateStatement->identifier, VARIABLE);
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

  std::string SystemC::createWait(parameters& parm, auto sensitivity) {
    std::string s = listToString(parm, sensitivity, " || ", [&](std::string s){return s + ".EVENT()";}); 
    if (s.size() == 0) {
      return "wait();";
    }
    return "wait([&](){return " + s + ";});";
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
                          println(parm, createWait(parm, method->sensitivity));
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
        signalAssignment(parm, s, [&](std::string baseName, std::string hierarchyName) {
            sensitivity.push_back(hierarchyName);
          });
        quiet = false;
        auto createBody = [&](parameters& parm) {
          createProcess(parm,
                        [&](parameters& parm) {
                          println(parm, createWait(parm, sensitivity));
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

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
    parm.addClassContents("void run();");
    std::string prefix = parm.hierarchyToString("::", true);
    parm.addImplementationContents("void " + prefix + "::run() {");
    func(parm);
    parm.addImplementationContents("}");
  }
  
  void SystemC::processDefinition(parameters& parm, ast::Process* process) {
    debug.functionStart("processDefinition");
    if (process) {
      std::string processName;
      if (process->label) {
        processName = process->label->toString(true);
      } else {
        processName = "noname" + std::to_string(processId++);
        process->noname = processName;
      }
      ast::ObjectType type = ast::ObjectType::PROCESS;
      std::string class_name = NameConverter::objectName(type, processName);
      auto createDefinition =
	[&](parameters& parm) {
	  createProcess(parm,
			[&](parameters& parm) {
			  sequentialStatements(parm, process->sequentialStatements);
			},
			class_name);
	};
      auto createBody = [&](parameters& parm) {
      };
      std::string class_description = "struct " + class_name;
      defineObject(parm, false, processName, type,
		   class_description, NULL,
                   &process->declarations, NULL, createBody, createDefinition, true, true);
      instantiateType(parm, processName, ast::ObjectType::PROCESS);
    }
    debug.functionEnd("processDefinition");
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
      std::list<std::string> sensitivity_list;
      auto sensitivityListGenerator =
	[&](DatabaseResult& object, std::string& name_extension) {
	  sensitivity_list.push_back(parm.getName(object, -1));
	  name_extension = ".read()";
	};
      auto createBody = [&](parameters& parm) {
			  createProcess(parm,
					[&](parameters& parm) {
					  signalAssignment(parm, s, sensitivityListGenerator);
					},
					name);
			};
      std::string class_description = "struct " + NameConverter::objectName(ast::ObjectType::PROCESS, name);
      defineObject(parm, false, name, ast::ObjectType::PROCESS,
		   class_description, NULL, NULL, NULL, createBody,
		   [&](parameters& parm) {}, true, true);
      instantiateType(parm, name, ast::ObjectType::PROCESS, &sensitivity_list);
      debug.functionEnd("concurrentSignalAssignment");
    }
  }
  
  void SystemC::concurrentStatementsDefinition(parameters& parm,
                                               ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    debug.functionStart("concurrentStatementsDefinition");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      processDefinition(parm, c.process);
      forGenerateStatementDefinition(parm, c.forGenerateStatement);
      blockStatementDefinition(parm, c.blockStatement);
      concurrentSignalAssignment(parm, c.signalAssignment);
    }
    debug.functionEnd("concurrentStatementsDefinition");
  }


}

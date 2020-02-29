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
      defineObject(parm, false, name, ast::ObjectType::BLOCK, "", 
		   &class_description, NULL, 
                   &blockStatement->declarations,
                   &blockStatement->concurrentStatements,
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
      std::string description_append = " + \"(\" + " + identifier + ".toString() + \")\"";
      parm.addClassConstructorContents("{");
      auto forloop_callback =
	[&](parameters& parm,
	    std::string& forloop_execution,
	    std::string& variable_instance,
	    std::string& variable_creation) {
	  parm.addClassConstructorContents(variable_instance);
	  parm.addClassConstructorContents(variable_creation);
	  parm.addClassConstructorContents(forloop_execution);
	  instantiateType(parm, name, ast::ObjectType::GENERATE, NULL, description_append);
	  parm.addClassConstructorContents("}");
	};
      forLoop(parm, identifier, forGenerateStatement->iteration, forloop_callback);
      parm.addClassConstructorContents("}");
      auto callback = [&](parameters& parm) {
      };
      defineObject(parm, false, name, ast::ObjectType::GENERATE, "",
		   NULL, &identifier,
                   &forGenerateStatement->declarations,
                   &forGenerateStatement->concurrentStatements,
		   callback, false, true);
    }
  }

  template <typename Func>
  void SystemC::createProcess(parameters& parm, Func func, std::string name) {
    parm.addClassContents("void run();");
    std::string prefix = parm.hierarchyToString("::", true);
    parm.addImplementationContents("void " + prefix + "::run() {");
    parm.addImplementationContents("do {");
    parm.addImplementationContents("if (m_static_sensitivity_exists && !m_spawn_method) {");
    parm.addImplementationContents(NameConverter::getTopLevelPrefix(parm) + "wait();");
    parm.addImplementationContents("}");
    func(parm);
    parm.addImplementationContents("} while(!m_spawn_method);");
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
      std::list<std::string> sensitivity_list;
      if (process->sensitivity) {
	for (auto& i : process->sensitivity->list) {
	  DatabaseResult result;
	  if (parm.findOne(result, &i)) {
	    sensitivity_list.push_back(NameConverter::getName(parm, result));
	  } 
	}
      }
      ast::ObjectType type = ast::ObjectType::PROCESS;
      std::string class_name = NameConverter::objectName(type, processName);
      auto callback =
	[&](parameters& parm) {
	  createProcess(parm,
			[&](parameters& parm) {
			  sequentialStatements(parm, process->sequentialStatements);
			},
			class_name);
	};
      std::string class_description = "struct " + class_name;
      defineObject(parm, false, processName, type, "",
		   &class_description, NULL,
                   &process->declarations, NULL, callback, true, true);
      instantiateType(parm, processName, ast::ObjectType::PROCESS, &sensitivity_list);
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
      auto callback =
	[&](parameters& parm) {
	  createProcess(parm,
			[&](parameters& parm) {
			  signalAssignment(parm, s, sensitivityListGenerator);
			},
			name);
	};
      defineObject(parm, false, name, ast::ObjectType::PROCESS, "", 
		   NULL, NULL, NULL, NULL, callback, true, true);
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

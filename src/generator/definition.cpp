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
      parm.addImplementationContents(getSourceLine(forGenerateStatement->name), __FILE__, __LINE__);
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

  void SystemC::createProcess(parameters& parm, ast::SimpleIdentifier* label, ast::Text* line,
			      std::list<std::string>* sensitivity_list,
			      bool sensitivity_all,
			      ast::List<ast::Declaration>* declarations,
			      ast::List<ast::SequentialStatement>& sequential_statements) {
    debug.functionStart("createProcess");
    std::string name;
    if (label) {
      name = label->toString(true);
    } else {
      name = "line" + std::to_string(line->getLine());
    }
    ast::ObjectType type = ast::ObjectType::PROCESS;
    std::string class_name = NameConverter::objectName(type, name);
    std::list<std::string> sequential_list;
    std::list<std::string> signal_list;
    auto signal_list_generator =
      [&](DatabaseResult& object, std::string& name_extension) {
	signal_list.push_back(parm.getName(object, -1));
	name_extension = ".read()";
      };
    auto callback =
      [&](parameters& parm) {
	parm.addClassContents("void run();");
	std::string prefix = parm.hierarchyToString("::", true);
	parm.addImplementationContents("void " + prefix + "::run() {", __FILE__, __LINE__);
    	sequentialStatements(parm, sequential_statements, sequential_list, signal_list_generator);
      };
    parm.process_contains_wait = false;
    std::string class_description = "struct " + class_name;
    defineObject(parm, false, name, type, "",
		 &class_description, NULL,
		 declarations, NULL, callback, true, true);
    bool spawn_method = !parm.process_contains_wait;
    std::list<std::string>* actual_sensitivity_list = (sensitivity_all ? &signal_list : sensitivity_list);
    instantiateType(parm, name, ast::ObjectType::PROCESS, actual_sensitivity_list);
    bool static_sensitivity_exists = (actual_sensitivity_list && !actual_sensitivity_list->empty()); 
    if (!spawn_method) {
      parm.addImplementationContents("do {", __FILE__, __LINE__);
    }
    if (static_sensitivity_exists && !spawn_method) {;
      parm.addImplementationContents(NameConverter::getTopLevelPrefix(parm, 1) + "wait();", __FILE__, __LINE__);
    }
    parm.addImplementationContents(sequential_list);
    if (!spawn_method) {
      parm.addImplementationContents("} while(true);", __FILE__, __LINE__);
    }
    parm.addImplementationContents("}", __FILE__, __LINE__);
    debug.functionStart("createProcess");
  }
  
  void SystemC::processDefinition(parameters& parm, ast::Process* process) {
    debug.functionStart("processDefinition");
    if (process) {
      std::list<std::string> sensitivity_list;
      if (process->sensitivity_list) {
	for (auto& i : process->sensitivity_list->list) {
	  DatabaseResult result;
	  if (parm.findOne(result, &i)) {
	    sensitivity_list.push_back(NameConverter::getName(parm, result));
	  } 
	}
      }
      createProcess(parm, process->label, process->process, &sensitivity_list,
		    process->sensitivity_all, &process->declarations,
		    process->sequentialStatements);
    }
    debug.functionEnd("processDefinition");
  }

  void SystemC::concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s) {
    if (s) {
      debug.functionStart("concurrentSignalAssignment");
      ast::List<ast::SequentialStatement> sequentialStatements;
      ast::SequentialStatement statement;
      statement.signalAssignment = s;
      sequentialStatements.add(&statement);
      std::string name;
      if (s->label) {
        name = s->label->toString(true);
      } else {
        name = "line" + std::to_string(s->target->getText().getLine());
      }
      createProcess(parm, s->label, &s->target->identifier->text, NULL,
		    true, NULL, sequentialStatements);
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

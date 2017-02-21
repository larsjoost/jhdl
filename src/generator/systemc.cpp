#include <iostream>
#include <list>
#include <cassert>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"
#include "../ast/interface.hpp"
#include "../ast/implementation.hpp"
#include "../ast/method.hpp"
#include "../ast/context_clause.hpp"
#include "../ast/expression.hpp"
#include "../ast/expression_term.hpp"
#include "../ast/declaration.hpp"
#include "../ast/variable_declaration.hpp"
#include "../ast/signal_declaration.hpp"
#include "../ast/constant_declaration.hpp"
#include "../ast/variable_assignment.hpp"
#include "../ast/signal_assignment.hpp"
#include "../ast/report_statement.hpp"

#include "systemc.hpp"

namespace generator {

  void SystemC::println(parameters& parm, std::string text) {
    std::cout << std::string(parm.indent, ' ') << text << std::endl;
  }
  
  SystemC::SystemC(ast::DesignFile& designFile) {
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    parameters parm;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.interface) {
        std::string name = toString(it->module.interface->name);
        println(parm, "#include \"systemc.h\"");
        println(parm, "#include \"vhdl.h\"");
        println(parm, "namespace vhdl {");
        parm.incIndent();
	println(parm, "using namespace STANDARD;");
        includes(parm, designFile);
        println(parm, "");
        println(parm, "SC_MODULE(" + name + ") {");
        println(parm, "public:");
        parm.parentName = name;
        implementation(parm, designFile, it->module.interface->name);
        println(parm, "};");
        parm.decIndent();
        println(parm, "}");
      }
      
    }
  }

  void SystemC::printSourceLine(parameters& parm, ast::Text& t) {
    println(parm, "// line " + std::to_string(t.getLine()) + ": " + t.getCurrentLine());
  }
  
  void SystemC::printSourceLine(parameters& parm, ast::BasicIdentifier* t) {
    printSourceLine(parm, t->text);
  }

  void SystemC::includes(parameters& parm, ast::DesignFile& designFile) {
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.contextClause) {
        for (ast::UseClause useClause : it->module.contextClause->useClauses.list) {
          println(parm, "using " + toString((char *)"::", useClause.list) + ";");
        }
      }
    }    
  }

  std::string SystemC::toString(const char* separator, ast::BasicIdentifierList* list) {
    std::string s = "";
    std::string delimiter = "";
    for (ast::BasicIdentifier t : list->textList.list) {
      s += delimiter + toString(&t);
      delimiter = separator;
    }
    return s;
  }

  void SystemC::basicIdentifierList(const char* separator, ast::BasicIdentifierList* list) {
    std::cout << toString(separator, list);
  }
  
  void SystemC::enumerationType(parameters parm, ast::BasicIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = toString(identifier);
      printSourceLine(parm, identifier);
      std::string enumName = name + "_enum";
      std::string basicIdentifierList = toString(", ", t->enumerations);
      println(parm, "enum " + enumName + " { " + basicIdentifierList + "};");
      println(parm, "class " + name + " : public Enumeration<" + enumName + "> {");
      println(parm, "public:"); 
      println(parm, "};");
    }
  }

  /*
  vhdl:
  type test_t is range 1 to 20;
  systemc:
  class test_t : public vhdl::Range<decltype(1)> {
  public:
    test_t(decltype(1) left=1, decltype(1) right=20) : vhdl::Range<decltype(1)>(left, right) {};
  };
  */

  void SystemC::numberType(parameters parm, ast::BasicIdentifier* identifier, ast::NumberType* t) {
    if (t) {
      std::string name = toString(identifier);
      std::string left = toString(t->range->left);
      std::string right = toString(t->range->right);
      std::string templateType = "decltype(" + left + ")"; 
      printSourceLine(parm, identifier);
      println(parm, "class " + name + " : public Range<" + templateType + "> {");
      println(parm, "public:"); 
      parm.incIndent();
      println(parm, "explicit " + name + "(" + templateType + " left=" + left +
              ", " + templateType + " right=" + right +
              ") : Range<" + templateType + ">(left, right) {};");
      parm.incIndent();
      println(parm, "using Range<" + templateType + ">::operator=;");
      parm.decIndent();
      println(parm, "};");
    }
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
    }
  }

  void SystemC::variable_declarations(parameters& parm, ast::VariableDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      println(parm, toString(v->type) + " " + toString(v->identifier) + ";");
    }
  }

  void SystemC::signal_declarations(parameters& parm, ast::SignalDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      std::string type = "sc_signal<" + toString(v->type) + "> ";
      std::string id = toString(v->identifier);
      parm.signalDeclaration[id] = type;
      println(parm, type + " " + id + ";");
    }
  }

  void SystemC::constant_declarations(parameters& parm, ast::ConstantDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      println(parm, "const " + toString(v->type) + " " + toString(v->identifier) + ";");
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      variable_declarations(parm, i.variable);
      signal_declarations(parm, i.signal);
      constant_declarations(parm, i.constant);
    }
  }

  void SystemC::sequentialStatements(parameters parm, ast::List<ast::SequentialStatement>& l) {
    for (ast::SequentialStatement s : l.list) {
      procedureCallStatement(parm, s.procedureCallStatement);
      variableAssignment(parm, s.variableAssignment);
      signalAssignment(parm, s.signalAssignment);
      reportStatement(parm, s.reportStatement);
      ifStatement(parm, s.ifStatement);
      forLoopStatement(parm, s.forLoopStatement);
      waitStatement(parm, s.waitStatement);
    }
  }

  template<class Key, class Value, typename Func>
  std::string SystemC::toString(std::unordered_map<Key, Value>& t, std::string delimiter, Func lambda) {
    std::string s;
    std::string d;
    for (auto x : t) {
      s += (d + lambda(x.first, x.second));
      d = delimiter;
    }
    return s;
  }

  template<class T, typename Func>
  std::string SystemC::toString(std::list<T>& t, std::string delimiter, Func lambda) {
    std::string s;
    std::string d;
    for (auto x : t) {
      s += (d + lambda(x));
      d = delimiter;
    }
    return s;
  }
  
  void SystemC::methodDefinition(parameters& parm, ast::Method* method) {
      if (method) {
        std::string methodName;
        if (method->name) {
          methodName = toString(method->name);
        } else {
          methodName = "noname" + std::to_string(methodId++);
          method->noname = methodName;
        }
        println(parm, "class " + methodName + " : public sc_thread {");
        parm.incIndent();
        for (auto s : parm.signalDeclaration) {
          println(parm, s.second + "& " + s.first + ";");
        }
        if (parm.forGenerateHierarchy.size() > 0) {
          println(parm, "INTEGER " + toString(parm.forGenerateHierarchy, ",", [&](std::string s){return s;}) + ";");
        }
        parm.decIndent();
        println(parm, "public:");
        parm.incIndent();
        std::string constructorArguments = "";
        std::string memberVariableAssignment = "";
        std::string colon = "";
        std::string comma = "";
        if (parm.forGenerateHierarchy.size() > 0) {
          colon = " : ";
          comma = ", ";
          constructorArguments = ", " + toString(parm.forGenerateHierarchy, ",", [&](std::string s){return "auto " + s;});
          memberVariableAssignment = toString(parm.forGenerateHierarchy, ",", [&](std::string s){return s + "(" + s + ")";});
        }
        std::string signalAssignment = "";
        if (parm.signalDeclaration.size() > 0) {
          colon = " : ";
          signalAssignment = toString(parm.signalDeclaration, ",", [&](std::string key, std::string value){return key + "(p->" + key + ")";});
        } else {
          comma = "";
        }
        std::string assignment = signalAssignment + comma + memberVariableAssignment;
        println(parm, methodName + "(" + parm.parentName +"* p" + constructorArguments + ")" + colon + assignment + " {};");
        println(parm, "void process() {");
        parm.incIndent();
        declarations(parm, method->declarations);
        parm.decIndent();
        sequentialStatements(parm, method->sequentialStatements);
        println(parm, "}");
        parm.decIndent();
        println(parm, "};");
      }
  }

  void SystemC::blockStatementDefinition(parameters& parm,
                                         ast::BlockStatement* blockStatement) {
    if (blockStatement) {
      println(parm, "class " + toString(blockStatement->name) + " {");
      declarations(parm, blockStatement->declarations);
      println(parm, "public:");
      concurrentStatementsDefinition(parm, blockStatement->concurrentStatements);
      println(parm, "};");
    }
  }
  
  void SystemC::forGenerateStatementDefinition(parameters parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      parm.forGenerateHierarchy.push_back(toString(forGenerateStatement->identifier));
      concurrentStatementsDefinition(parm, forGenerateStatement->concurrentStatements);
    }
  }
  
  void SystemC::concurrentStatementsDefinition(parameters parm,
                                               ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodDefinition(parm, c.method);
      forGenerateStatementDefinition(parm, c.forGenerateStatement);
      blockStatementDefinition(parm, c.blockStatement);
    }
  }
  
  void SystemC::methodInstantiation(parameters& parm, ast::Method* method) {
    if (method) {
      std::string methodName;
      if (method->name) {
        methodName = toString(method->name);
      } else {
        methodName = method->noname;
      }
      std::string constructorArguments = "";
      if (parm.forGenerateHierarchy.size() > 0) {
        constructorArguments = ", " + toString(parm.forGenerateHierarchy, ",", [&](std::string s){return s;});
      }
      println(parm, "SC_THREAD(new " + methodName + "(this" + constructorArguments + "));");
    }
  }

  void SystemC::blockStatementInstantiation(parameters parm,
                                            ast::BlockStatement* blockStatement) {
    
    if (blockStatement) {
      concurrentStatementsInstantiation(parm, blockStatement->concurrentStatements);
    }
  }

  void SystemC::forGenerateStatementInstantiation(parameters parm,
                                                  ast::ForGenerateStatement* forGenerateStatement) {
    
    if (forGenerateStatement) {
      std::string id = toString(forGenerateStatement->identifier);
      parm.forGenerateHierarchy.push_back(id);
      println(parm, toString(forGenerateStatement->identifier, forGenerateStatement->range) + ") {");
      concurrentStatementsInstantiation(parm, forGenerateStatement->concurrentStatements);
      println(parm, "}");
    }
  }
  
  void SystemC::concurrentStatementsInstantiation(parameters parm,
                                                  ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodInstantiation(parm, c.method);
      forGenerateStatementInstantiation(parm, c.forGenerateStatement);
      blockStatementInstantiation(parm, c.blockStatement);
    }
  }

  void SystemC::implementation(parameters& parm, ast::DesignFile& designFile, ast::BasicIdentifier* name) {
    for (ast::DesignUnit it : designFile.designUnits.list) {
      if (it.module.implementation) {
        if (name->equals(it.module.implementation->name)) {
          declarations(parm, it.module.implementation->declarations);
          concurrentStatementsDefinition(parm, it.module.implementation->concurrentStatements);
          println(parm, "public:");
          parm.incIndent();
          println(parm, "SC_CTOR(" + toString(name) + ") {");
          concurrentStatementsInstantiation(parm, it.module.implementation->concurrentStatements);
          println(parm, "}");
          parm.decIndent();
        }
      }
    }
  }

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      std::string s = "";
      if (p->associationList) {
        for (ast::AssociationElement e :
               p->associationList->associationElements.list) {
          s += toString(e.expression);
        }
      }
      println(parm, toString(p->name) + "(" + s + ");");
    }
  }
  
  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      println(parm, "report(" + toString(p->message) + ", " + toString(p->severity) + ");");
    }
  }

  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
	  println(parm, command + " (" + toString(c.condition) + ") {");
	} else {
	  println(parm, "} else {");
	}
	command = "} elsif";
        sequentialStatements(parm, c.sequentialStatements);
      }
      println(parm, "}");
    }
  }

  std::string SystemC::toString(ast::BasicIdentifier* i, ast::RangeType* r) {
    std::string s = "for (auto " + toString(i) + " : " + " INTEGER(" + toString(r->left) + ", " + toString(r->right) + ")";
    return s;
  }
  
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* p) {
    if (p) {
      println(parm, toString(p->identifier, p->range) + ") {");
      sequentialStatements(parm, p->sequentialStatements);
      println(parm, "}");
    }
  }

  std::string SystemC::toString(ast::Physical* p) {
    assert (p != NULL);
    return "physical(" + toString(p->number) + ", " +
      toString(p->unit) + ")";
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      println(parm, "wait(" + toString(p->physical->number) + ");");
    }
  }

  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      println(parm, toString(p->identifier) + " = " + toString(p->expression) + ";");
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      println(parm, toString(p->identifier) + " = " + toString(p->expression) + ";");
    }
  }

  std::string SystemC::toString(ast::BasicIdentifier* i) {
    assert (i != NULL);
    std::string s = i->text.toString(true);
    if (i->attribute) {
      s += "::" + i->attribute->toString(true);
      if (i->arguments) {
        s += "(" + toString((char *)",", i->arguments) + ")";
      }
    }
    return s;
  }

  void SystemC::basicIdentifier(ast::BasicIdentifier* i) {
    if (i) {      
      std::cout << toString(i) << std::endl;
    }
  }

  std::string SystemC::toString(ast::Number* n) {
    assert(n != NULL);
    return n->value.toString();
  }
  
  std::string SystemC::toString(ast::ExpressionTerm& e) {
    if (e.physical) {
      return toString(e.physical);
    }
    if (e.number) {
      return toString(e.number);
    }
    if (e.text) {
      return e.text->text.toString();
    }
    if (e.identifier) { 
      return toString(e.identifier);
    }
  }
  
  std::string SystemC::toString(ast::Expression* e) {
    assert(e != NULL);
    if (e->op) {
      std::string op;
      std::string term = toString(e->term);
      std::string expr = toString(e->expression);
      switch (e->op->op) {
      case ::ast::ExpressionOperator::CONCAT: {op = "concat"; break;}
      case ::ast::ExpressionOperator::ADD: {return term + " + " + expr;}
      case ::ast::ExpressionOperator::EQUAL: {op = "equal"; break;}
      default: {assert (false);}
      }
      return op + "(" + term + ", " + expr + ")";
    } else {
      return toString(e->term);
    }
  }
  
  void SystemC::expression(ast::Expression* e) {
    if (e) {
      std::cout << toString(e);
    }
  }
  
}

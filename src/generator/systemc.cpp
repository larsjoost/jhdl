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
#include "../ast/function_declaration.hpp"
#include "../ast/variable_assignment.hpp"
#include "../ast/signal_assignment.hpp"
#include "../ast/report_statement.hpp"

#include "systemc.hpp"

namespace generator {

  void SystemC::println(parameters& parm, std::string text) {
    std::cout << std::string(parm.indent, ' ') << text << std::endl;
  }
  
  void SystemC::functionStart(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION START] " << name << std::endl;
    }
  }

  SystemC::SystemC(bool verbose) : verbose(verbose) {}

  void SystemC::generate(ast::DesignFile& designFile) {
    functionStart("SystemC");
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    parameters parm;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.interface) {
        std::string name = basicIdentifierToString(parm, it->module.interface->name);
        println(parm, "#include \"systemc.h\"");
        println(parm, "#include \"vhdl.h\"");
        println(parm, "namespace vhdl {");
        parm.incIndent();
	println(parm, "using namespace STANDARD;");
        includes(parm, designFile);
        println(parm, "");
        println(parm, "SC_MODULE(" + name + ") {");
        println(parm, "public:");
        parm.incIndent();
        parm.parentName = name;
        implementation(parm, designFile, it->module.interface->name);
        parm.decIndent();
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
    functionStart("includes");
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.contextClause) {
        for (ast::UseClause useClause : it->module.contextClause->useClauses.list) {
          println(parm, "using " + listToString(parm, useClause.list, "::", [&](std::string s){return s;}) + ";");
        }
      }
    }    
  }

  void SystemC::enumerationType(parameters& parm, ast::BasicIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = basicIdentifierToString(parm, identifier);
      printSourceLine(parm, identifier);
      std::string enumName = name + "_enum";
      std::string basicIdentifierList = listToString(parm, t->enumerations, ", ",
                                                     [&](std::string s){return s;});
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

  void SystemC::numberType(parameters& parm, ast::BasicIdentifier* identifier, ast::NumberType* t) {
    if (t) {
      std::string name = basicIdentifierToString(parm, identifier);
      std::string left = expressionToString(parm, t->range->left);
      std::string right = expressionToString(parm, t->range->right);
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
      parm.decIndent();
    }
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
    }
  }

  std::string SystemC::variableDeclarationToString(parameters& parm, ast::VariableDeclaration* v) {
    std::string s = "";
    if (v) {
      s = basicIdentifierToString(parm, v->type) + " " + basicIdentifierToString(parm, v->identifier);
    }
    return s;
  }

  void SystemC::variable_declarations(parameters& parm, ast::VariableDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      println(parm, variableDeclarationToString(parm, v) + ";");
    }
  }

  std::string SystemC::signalDeclarationToString(parameters& parm, ast::SignalDeclaration* v) {
    std::string s = "";
    if (v) {
      printSourceLine(parm, v->identifier);
      DeclarationInfo i;
      std::string name = basicIdentifierToString(parm, v->identifier);
      std::string type = "sc_signal<" + basicIdentifierToString(parm, v->type) + ">"; 
      i.type = type + "& " + name;
      i.id = SIGNAL;
      parm.declaration[name] = i;
      println(parm, type + " " + name + ";");
    }
    return s;
  }

  void SystemC::signal_declarations(parameters& parm, ast::SignalDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      println(parm, signalDeclarationToString(parm, v) + ";");
    }
  }

  std::string SystemC::constantDeclarationToString(parameters& parm, ast::ConstantDeclaration* v) {
    std::string s = "";
    if (v) {
      s = "const " + basicIdentifierToString(parm, v->type) + " " +
        basicIdentifierToString(parm, v->identifier);
    }
    return s;
  }

  void SystemC::constant_declarations(parameters& parm, ast::ConstantDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      println(parm, constantDeclarationToString(parm, v) + ";");
    }
  }

  std::string SystemC::interfaceListToString(parameters& parm, ast::InterfaceList* l) {
    std::string s;
    if (l) {
      std::string x = "";
      std::string delimiter = "";
      for (ast::InterfaceElement i : l->interfaceElements.list) {
        if (i.variable) {x = variableDeclarationToString(parm, i.variable);}
        if (i.signal) {x = signalDeclarationToString(parm, i.signal);}
        if (i.constant) {x = constantDeclarationToString(parm, i.constant);}
        s += delimiter + x;
        delimiter = ", ";
      }
    }
    return s;
  }

  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f) {
    if (f) {
      printSourceLine(parm, f->name);
      std::string name = basicIdentifierToString(parm, f->name);
      std::string returnType = basicIdentifierToString(parm, f->returnType);
      std::string interface = "(" + interfaceListToString(parm, f->interface) + ")";
      DeclarationInfo i;
      i.type = returnType + " (*" + name  + ")" + interface;
      i.id = FUNCTION;
      parm.declaration[name] = i;
      println(parm, returnType + " " + name + interface + "{");
      parm.incIndent();
      declarations(parm, f->declarations);
      sequentialStatements(parm, f->sequentialStatements);
      parm.decIndent();
      println(parm, "}");
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      variable_declarations(parm, i.variable);
      signal_declarations(parm, i.signal);
      constant_declarations(parm, i.constant);
      function_declarations(parm, i.function);
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
      returnStatement(parm, s.returnStatement);
    }
  }

  template<class Key, class Value, typename Func>
  std::string SystemC::listToString(parameters& parm, std::unordered_map<Key, Value>& t, std::string delimiter, Func lambda) {
    std::string s;
    std::string d;
    for (auto x : t) {
      s += (d + lambda(x.first, x.second));
      d = delimiter;
    }
    return s;
  }

  template<class T, typename Func>
  std::string SystemC::listToString(parameters& parm, std::list<T>& t, std::string delimiter, Func lambda) {
    std::string s;
    std::string d;
    for (auto x : t) {
      s += (d + lambda(x));
      d = delimiter;
    }
    return s;
  }

  template<typename Func>
  std::string SystemC::listToString(parameters& parm, ast::BasicIdentifierList* list, std::string delimiter, Func lambda) {
    std::string s = "";
    if (list) {
      std::string d = "";
      for (ast::BasicIdentifier t : list->textList.list) {
        s += (d + lambda(basicIdentifierToString(parm, &t)));
        d = delimiter;
      }
    }
    return s;
  }
  
  std::string SystemC::getConstructorDeclaration(parameters& parm, std::string& name) {
    std::string constructorArguments = "";
    std::string memberVariableAssignment = "";
    std::string colon = "";
    std::string comma = "";
    if (parm.forGenerateHierarchy.size() > 0) {
      colon = " : ";
      comma = ", ";
      constructorArguments = ", " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return "auto " + s;});
      memberVariableAssignment = listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return s + "(" + s + ")";});
    }
    std::string signalAssignment = "";
    if (parm.declaration.size() > 0) {
      colon = " : ";
      signalAssignment = listToString(parm, parm.declaration, ",", [&](std::string key, DeclarationInfo value){return key + "(p->" + key + ")";});
    } else {
      comma = "";
    }
    std::string assignment = signalAssignment + comma + memberVariableAssignment;
    return name + "(" + parm.parentName +"* p" + constructorArguments + ")" + colon + assignment;
  }
  
  void SystemC::methodDefinition(parameters& parm, ast::Method* method) {
    functionStart("methodDefinition");
    if (method) {
      std::string methodName;
      if (method->name) {
        methodName = basicIdentifierToString(parm, method->name);
      } else {
        methodName = "noname" + std::to_string(methodId++);
        method->noname = methodName;
      }
      println(parm, "class " + methodName + " : public sc_thread {");
      parm.incIndent();
      for (auto s : parm.declaration) {
        println(parm, s.second.type + ";");
      }
      if (parm.forGenerateHierarchy.size() > 0) {
        println(parm, "INTEGER " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return s;}) + ";");
      }
      parm.decIndent();
      println(parm, "public:");
      parm.incIndent();
      println(parm, getConstructorDeclaration(parm, methodName) +  + " {};");
      println(parm, "void process() {");
      parm.incIndent();
      if (method->sensitivity) {
        std::string s = listToString(parm, method->sensitivity, " || ", [&](std::string s){return s + ".EVENT()";}); 
        println(parm, "wait([&](){return " + s + ";});");
      }
      declarations(parm, method->declarations);
      sequentialStatements(parm, method->sequentialStatements);
      parm.decIndent();
      println(parm, "}");
      parm.decIndent();
      println(parm, "};");
    }
  }

  void SystemC::blockStatementDefinition(parameters parm,
                                         ast::BlockStatement* blockStatement) {
    functionStart("blockStatementDefinition");
    if (blockStatement) {
      std::string name = basicIdentifierToString(parm, blockStatement->name);
      printSourceLine(parm, blockStatement->name);
      println(parm, "SC_BLOCK(" + basicIdentifierToString(parm, blockStatement->name) + ") {");
      parm.incIndent();
      {
        parameters p = parm;
        declarations(p, blockStatement->declarations);
        println(parm, "public:");
        p.parentName = name;
        concurrentStatementsDefinition(p, blockStatement->concurrentStatements);
      }
      println(parm, getConstructorDeclaration(parm, name) + "{");
      concurrentStatementsInstantiation(parm, blockStatement->concurrentStatements);
      println(parm, "}");
      parm.decIndent();
      println(parm, "};");
    }
  }
  
  void SystemC::forGenerateStatementDefinition(parameters parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      parm.forGenerateHierarchy.push_back(basicIdentifierToString(parm, forGenerateStatement->identifier));
      concurrentStatementsDefinition(parm, forGenerateStatement->concurrentStatements);
    }
  }
  
  void SystemC::concurrentStatementsDefinition(parameters parm,
                                               ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    functionStart("concurrentStatementsDefinition");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodDefinition(parm, c.method);
      forGenerateStatementDefinition(parm, c.forGenerateStatement);
      blockStatementDefinition(parm, c.blockStatement);
    }
  }

  void SystemC::instantiateType(parameters& parm, std::string type, std::string name) {
    functionStart("instantiateType");
    std::string constructorArguments = "";
    if (parm.forGenerateHierarchy.size() > 0) {
      constructorArguments = ", " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return s;});
    }
    println(parm, type + "(new " + name + "(this" + constructorArguments + "));");
  }
  
  void SystemC::methodInstantiation(parameters& parm, ast::Method* method) {
    functionStart("methodInstantiation");
    if (method) {
      std::string methodName;
      if (method->name) {
        methodName = basicIdentifierToString(parm, method->name);
      } else {
        methodName = method->noname;
      }
      instantiateType(parm, "SC_THREAD", methodName);
    }
  }

  void SystemC::blockStatementInstantiation(parameters parm,
                                            ast::BlockStatement* blockStatement) {
    functionStart("blockStatementInstantiation");
    if (blockStatement) {
      instantiateType(parm, "SC_NEW_BLOCK", basicIdentifierToString(parm, blockStatement->name));
    }
  }

  void SystemC::forGenerateStatementInstantiation(parameters parm,
                                                  ast::ForGenerateStatement* forGenerateStatement) {
    
    if (forGenerateStatement) {
      std::string id = basicIdentifierToString(parm, forGenerateStatement->identifier);
      parm.forGenerateHierarchy.push_back(id);
      println(parm, rangeTypeToString(parm, forGenerateStatement->identifier, forGenerateStatement->range) + ") {");
      concurrentStatementsInstantiation(parm, forGenerateStatement->concurrentStatements);
      println(parm, "}");
    }
  }
  
  void SystemC::concurrentStatementsInstantiation(parameters parm,
                                                  ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    functionStart("concurrentStatementsInstantiation");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodInstantiation(parm, c.method);
      forGenerateStatementInstantiation(parm, c.forGenerateStatement);
      blockStatementInstantiation(parm, c.blockStatement);
    }
  }

  void SystemC::threadConstructor(parameters parm, ast::BasicIdentifier* name, 
                                  ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    println(parm, "SC_CTOR(" + basicIdentifierToString(parm, name) + ") {");
    concurrentStatementsInstantiation(parm, concurrentStatements);
    println(parm, "}");
  }
  
  void SystemC::implementation(parameters& parm, ast::DesignFile& designFile, ast::BasicIdentifier* name) {
    functionStart("implementation");
    for (ast::DesignUnit it : designFile.designUnits.list) {
      if (it.module.implementation) {
        if (name->equals(it.module.implementation->name)) {
          declarations(parm, it.module.implementation->declarations);
          concurrentStatementsDefinition(parm, it.module.implementation->concurrentStatements);
          println(parm, "public:");
          parm.incIndent();
          threadConstructor(parm, name, it.module.implementation->concurrentStatements);
          parm.decIndent();
        }
      }
    }
  }

  std::string SystemC::procedureCallStatementToString(parameters& parm, ast::ProcedureCallStatement* p) {
    std::string s = "";
    if (p) {
      if (p->associationList) {
        for (ast::AssociationElement e :
               p->associationList->associationElements.list) {
          s += expressionToString(parm, e.expression);
        }
      }
      s = basicIdentifierToString(parm, p->name) + "(" + s + ");";
    }
    return s;
  }

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      println(parm, procedureCallStatementToString(parm, p));
    }
  }
  
  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      println(parm, "report(" + expressionToString(parm, p->message) + ", " +
              basicIdentifierToString(parm, p->severity) + ");");
    }
  }

  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
	  println(parm, command + " (" + expressionToString(parm, c.condition) + ") {");
	} else {
	  println(parm, "} else {");
	}
	command = "} elsif";
        sequentialStatements(parm, c.sequentialStatements);
      }
      println(parm, "}");
    }
  }

  std::string SystemC::rangeTypeToString(parameters& parm, ast::BasicIdentifier* i, ast::RangeType* r) {
    std::string s = "for (auto " + basicIdentifierToString(parm, i) + " : " +
      " INTEGER(" + expressionToString(parm, r->left) + ", " +
      expressionToString(parm, r->right) + ")";
    return s;
  }
  
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* p) {
    if (p) {
      println(parm, rangeTypeToString(parm, p->identifier, p->range) + ") {");
      sequentialStatements(parm, p->sequentialStatements);
      println(parm, "}");
    }
  }

  std::string SystemC::physicalToString(parameters& parm, ast::Physical* p) {
    assert (p != NULL);
    return "physical(" + numberToString(parm, p->number) + ", " +
      basicIdentifierToString(parm, p->unit) + ")";
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      println(parm, "wait(" + numberToString(parm, p->physical->number) + ");");
    }
  }

  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      println(parm, basicIdentifierToString(parm, p->identifier) + " = " +
              expressionToString(parm, p->expression) + ";");
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
      println(parm, "return " + expressionToString(parm, r->value) + ";");
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      println(parm, basicIdentifierToString(parm, p->identifier) + " = " +
              expressionToString(parm, p->expression) + ";");
    }
  }

  bool SystemC::matchDeclarationID(parameters& parm, std::string name, DeclarationID id) {
    auto t = parm.declaration.find(name); 
    if (t == parm.declaration.end()) {
      return false;
    }
    return t->second.id == id;
  }
  
  std::string SystemC::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* i) {
    assert (i != NULL);
    std::string s = i->text.toString(true);
    if (i->attribute) {
      std::string seperator = matchDeclarationID(parm, s, SIGNAL) ? "." : "::";
      s += seperator + i->attribute->toString(true);
      s += "(" + listToString(parm, i->arguments, ",", [&](std::string s){return s;}) + ")";
    }
    return s;
  }

  std::string SystemC::numberToString(parameters& parm, ast::Number* n) {
    assert(n != NULL);
    return n->value.toString();
  }
  
  std::string SystemC::characterToString(parameters& parm, ast::Character* n) {
    assert(n != NULL);
    return n->value.toString();
  }
  
  std::string SystemC::expressionTermToString(parameters& parm, ast::ExpressionTerm* e) {
    if (e) {
      if (e->physical) {
        return physicalToString(parm, e->physical);
      }
      if (e->number) {
        return numberToString(parm, e->number);
      }
      if (e->text) {
        return e->text->text.toString();
      }
      if (e->identifier) { 
        return basicIdentifierToString(parm, e->identifier);
      }
      if (e->character) {
        return characterToString(parm, e->character);
      }
      if (e->procedureCall) {
        return procedureCallStatementToString(parm, e->procedureCall);
      }
    }
    return "";
  }
  
  std::string SystemC::expressionToString(parameters& parm, ast::Expression* e) {
    assert(e != NULL);
    if (e->parenthis) {
      return "(" + expressionToString(parm, e->parenthis) + ")";
    } else if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(parm, e->expression);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      default: {assert (false);}
      }
      return op + expr;
    } else if (e->op) {
      std::string op;
      std::string term = expressionTermToString(parm, e->term);
      std::string expr = expressionToString(parm, e->expression);
      switch (e->op->op) {
      case ::ast::ExpressionOperator::CONCAT: {op = "concat"; break;}
      case ::ast::ExpressionOperator::ADD: {return term + " + " + expr;}
      case ::ast::ExpressionOperator::EQUAL: {return term + " == " + expr;}
      case ::ast::ExpressionOperator::NOT_EQUAL: {return term + " != " + expr;}
      case ::ast::ExpressionOperator::LARGER_OR_EQUAL: {return term + " >= " + expr;}
      case ::ast::ExpressionOperator::SMALLER_OR_EQUAL: {return term + " <= " + expr;}
      default: {assert (false);}
      }
      return op + "(" + term + ", " + expr + ")";
    } else {
      return expressionTermToString(parm, e->term);
    }
  }
  
}

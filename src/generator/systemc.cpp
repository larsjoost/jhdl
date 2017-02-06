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

  SystemC::SystemC(ast::DesignFile& designFile) {
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.interface) {
        std::string name = toString(it->module.interface->name);
        std::cout << "#include \"systemc.h\"" << std::endl;
        std::cout << "#include \"vhdl.h\"" << std::endl;
	std::cout << "namespace vhdl {" << std::endl;
	std::cout << "using namespace STANDARD;" << std::endl;
        includes(indentSpace, designFile);
        std::cout << "SC_MODULE(" << name << ") {" << std::endl;
        implementation(indentSpace, designFile, it->module.interface->name);
        std::cout << "};" << std::endl;
        std::cout << "}" << std::endl;
      }
      
    }
  }

  void SystemC::printSourceLine(ast::Text& t) {
    std::cout << "/*" << std::endl;
    t.printLinePosition();
    std::cout << "*/" << std::endl;
  }
  
  void SystemC::printSourceLine(ast::BasicIdentifier* t) {
    printSourceLine(t->text);
  }

  void SystemC::includes(int indent, ast::DesignFile& designFile) {
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.contextClause) {
        for (ast::UseClause useClause : it->module.contextClause->useClauses.list) {
          std::cout << std::string(indent, ' ') << "using ";
	  std::cout << toString((char *)"::", useClause.list);
          std::cout << ";" << std::endl;
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
  
  void SystemC::enumerationType(ast::BasicIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = toString(identifier);
      printSourceLine(identifier);
      std::string enumName = name + "_enum";
      std::cout << "enum " << enumName << " { ";
      basicIdentifierList(", ", t->enumerations);
      std::cout << "};" << std::endl;
      std::cout << "class " << name << " : public Enumeration<" << enumName << "> {" << std::endl;
      std::cout << "  public:" << std::endl; 
      std::cout << "};" << std::endl;
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

  void SystemC::numberType(ast::BasicIdentifier* identifier, ast::NumberType* t) {
    if (t) {
      std::string name = toString(identifier);
      std::string left = toString(t->range->left);
      std::string right = toString(t->range->right);
      std::string templateType = "decltype(" + left + ")"; 
      printSourceLine(identifier);
      std::cout << "class " << name << " : public Range<" << templateType << "> {" << std::endl;
      std::cout << "  public:" << std::endl; 
      std::cout << "  explicit " << name << "(" << templateType << " left=" << left;
      std::cout << ", " << templateType << " right=" << right;
      std::cout << ") : Range<" << templateType << ">(left, right) {};" << std::endl;
      std::cout << "  using Range<" << templateType + ">::operator=;" << std::endl;
      std::cout << "};" << std::endl;
    }
  }

  void SystemC::type_declarations(ast::TypeDeclaration* t) {
    if (t) {
      assert (t->typeDefinition);
      numberType(t->identifier, t->typeDefinition->numberType);
      enumerationType(t->identifier, t->typeDefinition->enumerationType);
    }
  }

  void SystemC::variable_declarations(ast::VariableDeclaration* v) {
    if (v) {
      printSourceLine(v->identifier);
      std::cout << toString(v->type) << " " << toString(v->identifier) << ";" << std::endl;
    }
  }

  void SystemC::signal_declarations(ast::SignalDeclaration* v) {
    if (v) {
      printSourceLine(v->identifier);
      std::cout << toString(v->type) << " " << toString(v->identifier) << ";" << std::endl;
    }
  }

  void SystemC::constant_declarations(ast::ConstantDeclaration* v) {
    if (v) {
      printSourceLine(v->identifier);
      std::cout << "const " << toString(v->type) << " " << toString(v->identifier) << ";" << std::endl;
    }
  }

  void SystemC::declarations(ast::List<ast::Declaration>& d) {
    for (ast::Declaration i : d.list) {
      type_declarations(i.type);
      variable_declarations(i.variable);
      signal_declarations(i.signal);
      constant_declarations(i.constant);
    }
  }

  void SystemC::sequentialStatements(ast::List<ast::SequentialStatement>& l) {
    for (ast::SequentialStatement s : l.list) {
      procedureCallStatement(s.procedureCallStatement);
      variableAssignment(s.variableAssignment);
      signalAssignment(s.signalAssignment);
      reportStatement(s.reportStatement);
      ifStatement(s.ifStatement);
      forLoopStatement(s.forLoopStatement);
      waitStatement(s.waitStatement);
    }
  }
  
  
  void SystemC::implementation(int indent, ast::DesignFile& designFile, ast::BasicIdentifier* name) {
    int methodId = 0;
    std::list<std::string> methodNames;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.implementation) {
        if (name->equals(it->module.implementation->name)) {
          declarations(it->module.implementation->declarations);
          for (ast::Method m : it->module.implementation->methods.list) {
            std::string methodName;
            if (m.name) {
              methodName = toString(m.name);
            } else {
              methodName = "noname" + std::to_string(methodId++);
            }
            methodNames.push_back(methodName);
            std::cout << "void " << methodName << "() {" << std::endl;
            std::cout << "while(true) {" << std::endl;
            declarations(m.declarations);
            sequentialStatements(m.sequentialStatements);
            std::cout << "}" << std::endl;
            std::cout << "}" << std::endl;
          }
          std::cout << "public:" << std::endl;
          std::cout << toString(name) << "() {" << std::endl;
          for (std::string s : methodNames) {
            std::cout << "  SC_METHOD(&" + toString(name) + "::" + s + ");" << std::endl;
          }
          std::cout << "}" << std::endl;
        }
      }
    }
  }

  void SystemC::procedureCallStatement(ast::ProcedureCallStatement* p) {
    if (p) {
      std::cout << toString(p->name) << "(";
      if (p->associationList) {
        for (ast::AssociationElement e :
               p->associationList->associationElements.list) {
          expression(e.expression);
        }
      }
      std::cout << ");" << std::endl;
    }
  }
  
  void SystemC::reportStatement(ast::ReportStatement* p) {
    if (p) {
      std::cout << "report(";
      expression(p->message);
      std::cout << ", ";
      std::cout << toString(p->severity) << ");" << std::endl;
    }
  }

  void SystemC::ifStatement(ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
	  std::cout << command << " (" << toString(c.condition) << ") {" << std::endl;
	} else {
	  std::cout << "} else {" << std::endl;
	}
	command = "} elsif";
        sequentialStatements(c.sequentialStatements);
      }
      std::cout << "}" << std::endl;
    }
  }

  void SystemC::forLoopStatement(ast::ForLoopStatement* p) {
    if (p) {
      std::cout << "for (auto " << toString(p->identifier) << " : ";
      std::cout << " INTEGER(" << toString(p->range->left) << ", ";
      std::cout << toString(p->range->right) << ")) {" << std::endl;
      sequentialStatements(p->sequentialStatements);
      std::cout << "}" << std::endl;
    }
  }

  std::string SystemC::toString(ast::Physical* p) {
    assert (p != NULL);
    return "physical(" + toString(p->number) + ", " +
      toString(p->unit) + ")";
  }
  
  void SystemC::waitStatement(ast::WaitStatement* p) {
    if (p) {
      std::cout << "wait(" << toString(p->physical->number) << ");" << std::endl;
    }
  }

  void SystemC::variableAssignment(ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(p->identifier);
      std::cout << toString(p->identifier) << " = ";
      expression(p->expression);
      std::cout << ";" << std::endl;
    }
  }

  void SystemC::signalAssignment(ast::SignalAssignment* p) {
    if (p) {
      printSourceLine(p->identifier);
      std::cout << toString(p->identifier) << " = ";
      expression(p->expression);
      std::cout << ";" << std::endl;
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

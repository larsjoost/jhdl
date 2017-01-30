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
#include "../ast/variable_assignment.hpp"
#include "../ast/report_statement.hpp"

#include "design_file.hpp"

namespace generator {

  DesignFile::DesignFile(ast::DesignFile& designFile) {
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.interface) {
        std::string name = it->module.interface->name.toString();
        std::cout << "#include \"systemc.h\"" << std::endl;
        std::cout << "#include \"vhdl.h\"" << std::endl;
	std::cout << "using namespace vhdl;" << std::endl;
	std::cout << "using namespace vhdl::STANDARD;" << std::endl;
        includes(designFile);
        std::cout << "SC_MODULE(" << name << ")" << std::endl;
        std::cout << "{" << std::endl;
        implementation(designFile, it->module.interface->name);
        std::cout << "};" << std::endl;
        std::cout << "int sc_main(int argc, char* argv[]) {" << std::endl;
        std::cout << "  " << name << " a;" << std::endl;
        std::cout << "}" << std::endl;
      }
      
    }
  }

  void DesignFile::printSourceLine(ast::Text& t) {
    std::cout << "/*" << std::endl;
    t.printLinePosition();
    std::cout << "*/" << std::endl;
  }
  
  void DesignFile::printSourceLine(ast::BasicIdentifier* t) {
    printSourceLine(t->text);
  }

  void DesignFile::includes(ast::DesignFile& designFile) {
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.contextClause) {
        for (ast::UseClause useClause : it->module.contextClause->useClauses.list) {
          std::cout << "using vhdl::";
	  std::cout << toString((char *)"::", useClause.list);
          std::cout << ";" << std::endl;
        }
      }
    }    
  }

  std::string DesignFile::toString(const char* separator, ast::BasicIdentifierList* list) {
    std::string s = "";
    std::string delimiter = "";
    for (ast::BasicIdentifier t : list->textList.list) {
      s += delimiter + toString(&t);
      delimiter = separator;
    }
    return s;
  }

  void DesignFile::basicIdentifierList(const char* separator, ast::BasicIdentifierList* list) {
    std::cout << toString(separator, list);
  }
  
  void DesignFile::enumerationType(ast::Text& text, ast::EnumerationType* t) {
    if (t) {
      std::string name = text.toString();
      printSourceLine(text);
      std::string enumName = name + "_enum";
      std::cout << "enum " << enumName << " { ";
      basicIdentifierList(", ", t->enumerations);
      std::cout << "};" << std::endl;
      std::cout << "class " << name << " : public vhdl::Enumeration<" << enumName << "> {" << std::endl;
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

  void DesignFile::numberType(ast::Text& text, ast::NumberType* t) {
    if (t) {
      std::string name = text.toString();
      std::string left = toString(t->left);
      std::string right = toString(t->right);
      std::string templateType = "decltype(" + left + ")"; 
      printSourceLine(text);
      std::cout << "class " << name << " : public vhdl::Range<" << templateType << "> {" << std::endl;
      std::cout << "  public:" << std::endl; 
      std::cout << "  explicit " << name << "(" << templateType << " left=" << left;
      std::cout << ", " << templateType << " right=" << right;
      std::cout << ") : vhdl::Range<" << templateType << ">(left, right) {};" << std::endl;
      std::cout << "  using vhdl::Range<" << templateType + ">::operator=;" << std::endl;
      std::cout << "};" << std::endl;
    }
  }

  void DesignFile::type_declarations(ast::TypeDeclaration* t) {
    if (t) {
      assert (t->typeDefinition);
      numberType(t->identifier, t->typeDefinition->numberType);
      enumerationType(t->identifier, t->typeDefinition->enumerationType);
    }
  }

  void DesignFile::variable_declarations(ast::VariableDeclaration* v) {
    if (v) {
      printSourceLine(v->identifier);
      std::cout << toString(v->type) << " " << toString(v->identifier) << ";" << std::endl;
    }
  }

  void DesignFile::declarations(ast::List<ast::Declaration>& d) {
    for (ast::Declaration i : d.list) {
      type_declarations(i.type);
      variable_declarations(i.variable);
    }
  }

  void DesignFile::implementation(ast::DesignFile& designFile, ast::Text& name) {
    int methodId = 0;
    std::list<std::string> methodNames;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.implementation) {
        if (name.equals(it->module.implementation->name)) {
          declarations(it->module.implementation->declarations);
          for (ast::Method m : it->module.implementation->methods.list) {
            std::string methodName;
            if (m.name) {
              methodName = m.name->toString();
            } else {
              methodName = "noname" + std::to_string(methodId++);
            }
            methodNames.push_back(methodName);
            std::cout << "void " << methodName << "() {" << std::endl;
            declarations(m.declarations);
            for (ast::SequentialStatement s : m.sequentialStatements.list) {
              procedureCallStatement(s.procedureCallStatement);
              variableAssignment(s.variableAssignment);
              reportStatement(s.reportStatement);
            }
            std::cout << "}" << std::endl;
          }
          std::cout << "public:" << std::endl;
          std::cout << "SC_CTOR(" << name.toString() << ") {" << std::endl;
          for (std::string s : methodNames) {
            std::cout << "  SC_METHOD(" + s + ");" << std::endl;
          }
          std::cout << "}" << std::endl;
        }
      }
    }
  }

  void DesignFile::procedureCallStatement(ast::ProcedureCallStatement* p) {
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
  
  void DesignFile::reportStatement(ast::ReportStatement* p) {
    if (p) {
      std::cout << "vhdl::REPORT(";
      expression(p->message);
      std::cout << ", ";
      std::cout << p->severity.toString() << ");" << std::endl;
    }
  }

  void DesignFile::variableAssignment(ast::VariableAssignment* p) {
    if (p) {
      std::cout << toString(p->identifier) << " = ";
      expression(p->expression);
      std::cout << ";" << std::endl;
    }
  }

  std::string DesignFile::toString(ast::BasicIdentifier* i) {
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

  void DesignFile::basicIdentifier(ast::BasicIdentifier* i) {
    if (i) {      
      std::cout << toString(i) << std::endl;
    }
  }

  std::string DesignFile::toString(ast::ExpressionTerm& e) {
    if (e.number) {
      return e.number->value.toString();
    }
    if (e.text) {
      return e.text->text.toString();
    }
    if (e.identifier) { 
      return toString(e.identifier);
    }
  }
  
  std::string DesignFile::toString(ast::Expression* e) {
    assert(e != NULL);
    if (e->op) {
      std::string op;
      if (e->op->op == "&") {
        op = "CONCAT";
      }
      return op + "(" + toString(e->term) + ", " + toString(e->expression) + ")";
    } else {
      return toString(e->term);
    }
  }
  
  void DesignFile::expression(ast::Expression* e) {
    if (e) {
      std::cout << toString(e);
    }
  }
  
}

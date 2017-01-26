#include <iostream>
#include <list>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"
#include "../ast/interface.hpp"
#include "../ast/implementation.hpp"
#include "../ast/method.hpp"
#include "../ast/context_clause.hpp"
#include "../ast/expression.hpp"
#include "../ast/declaration.hpp"
#include "../ast/variable_declaration.hpp"
#include "../ast/variable_declaration.hpp"

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
  
  void DesignFile::includes(ast::DesignFile& designFile) {
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.contextClause) {
        for (ast::UseClause useClause : it->module.contextClause->useClauses.list) {
          std::cout << "using vhdl";
          for (ast::Text t : useClause.list.list) {
            std::cout << "::" << t.toString();
          }
          std::cout << ";" << std::endl;
        }
      }
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

  void DesignFile::type_declarations(ast::TypeDeclaration* t) {
    if (t) {
      printSourceLine(t->identifier);
      std::string name = t->identifier.toString();
      std::cout << "class " << name << " : public vhdl::Range<decltype(";
      expression(t->left);
      std::cout << ")> {" << std::endl;
      std::cout << "  public:" << std::endl; 
      std::cout << "  " << name << "(decltype(";
      expression(t->left);
      std::cout << ") left=";
      expression(t->left);
      std::cout << ", decltype(";
      expression(t->left);
      std::cout << ") right=";
      expression(t->right);
      std::cout << ") : vhdl::Range<decltype(";
      expression(t->left);
      std::cout << ")>(left, right) {};" << std::endl;
      std::cout << "};" << std::endl;
    }
  }

  void DesignFile::variable_declarations(ast::VariableDeclaration* v) {
    if (v) {
      printSourceLine(v->identifier);
      std::cout << v->type.toString() << " " << v->identifier.toString() << ";" << std::endl;
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
      std::cout << p->name.toString() << "(";
      if (p->associationList) {
        for (ast::AssociationElement e :
               p->associationList->associationElements.list) {
          expression(e.expression);
        }
      }
      std::cout << ");" << std::endl;
    }
  }
  
  void DesignFile::expression(ast::Expression* e) {
    if (e) {
      if (e->number) {
        std::cout << e->number->value.toString();
      }
    }
  }
  
}

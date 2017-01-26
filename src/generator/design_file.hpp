#ifndef GENERATOR_DESIGNFILE_HPP_
#define GENERATOR_DESIGNFILE_HPP_

#include "../ast/text.hpp"
#include "../ast/design_file.hpp"
#include "../ast/expression.hpp"
#include "../ast/procedure_call_statement.hpp"
#include "../ast/variable_assignment.hpp"

namespace generator {
  
  class DesignFile { 
    std::string toString(ast::Expression* e);
    void expression(ast::Expression* e);
    void procedureCallStatement(ast::ProcedureCallStatement* p);
    void variableAssignment(ast::VariableAssignment* p);
    void includes(ast::DesignFile& designFile);
    void type_declarations(ast::TypeDeclaration* t);
    void variable_declarations(ast::VariableDeclaration* v);
    void declarations(ast::List<ast::Declaration>& d);
    void implementation(ast::DesignFile& designFile, ast::Text& name);

    void printSourceLine(ast::Text& t);
  public:
    DesignFile(ast::DesignFile& designFile);
  };
}

#endif



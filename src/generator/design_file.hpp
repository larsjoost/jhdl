#ifndef GENERATOR_DESIGNFILE_HPP_
#define GENERATOR_DESIGNFILE_HPP_

#include "../ast/design_file.hpp"
#include "../ast/expression.hpp"
#include "../ast/procedure_call_statement.hpp"

namespace generator {
  
  class DesignFile { 
    void expression(ast::Expression* e);
    void procedureCallStatement(ast::ProcedureCallStatement* p);
    void implementation(ast::DesignFile& designFile, ast::Text& name);
  public:
    DesignFile(ast::DesignFile& designFile);
  };
}

#endif



#ifndef GENERATOR_DESIGNFILE_HPP_
#define GENERATOR_DESIGNFILE_HPP_

#include "../ast/text.hpp"
#include "../ast/design_file.hpp"
#include "../ast/expression_term.hpp"
#include "../ast/expression.hpp"
#include "../ast/procedure_call_statement.hpp"
#include "../ast/variable_assignment.hpp"
#include "../ast/enumeration_type.hpp"
#include "../ast/number_type.hpp"
#include "../ast/basic_identifier.hpp"
#include "../ast/basic_identifier_list.hpp"
#include "../ast/report_statement.hpp"

namespace generator {
  
  class DesignFile { 
    std::string toString(ast::ExpressionTerm& e);
    std::string toString(ast::Expression* e);
    void expression(ast::Expression* e);
    std::string toString(ast::BasicIdentifier* i);
    void basicIdentifier(ast::BasicIdentifier* i);
    std::string toString(const char* separator, ast::BasicIdentifierList* list);
    void basicIdentifierList(const char* separator, ast::BasicIdentifierList* list);

    void reportStatement(ast::ReportStatement* p);
    void procedureCallStatement(ast::ProcedureCallStatement* p);
    void variableAssignment(ast::VariableAssignment* p);
    void includes(ast::DesignFile& designFile);

    void numberType(ast::Text& text, ast::NumberType* t);
    void enumerationType(ast::Text& text, ast::EnumerationType* t);
    void type_declarations(ast::TypeDeclaration* t);

    void variable_declarations(ast::VariableDeclaration* v);
    void declarations(ast::List<ast::Declaration>& d);

    void implementation(ast::DesignFile& designFile, ast::BasicIdentifier* name);

    void printSourceLine(ast::Text& t);
    void printSourceLine(ast::BasicIdentifier* t);
  public:
    DesignFile(ast::DesignFile& designFile);
  };
}

#endif



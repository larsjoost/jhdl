#ifndef GENERATOR_SYSTEMC_HPP_
#define GENERATOR_SYSTEMC_HPP_

#include "../ast/text.hpp"
#include "../ast/design_file.hpp"
#include "../ast/expression_term.hpp"
#include "../ast/expression.hpp"
#include "../ast/procedure_call_statement.hpp"
#include "../ast/variable_assignment.hpp"
#include "../ast/signal_declaration.hpp"
#include "../ast/constant_declaration.hpp"
#include "../ast/enumeration_type.hpp"
#include "../ast/number_type.hpp"
#include "../ast/basic_identifier.hpp"
#include "../ast/basic_identifier_list.hpp"
#include "../ast/report_statement.hpp"
#include "../ast/if_statement.hpp"
#include "../ast/forloop_statement.hpp"
#include "../ast/wait_statement.hpp"
#include "../ast/sequential_statement.hpp"

namespace generator {
  
  class SystemC { 

    const int indentSpace = 2;

    std::string toString(ast::ExpressionTerm& e);
    std::string toString(ast::Expression* e);
    void expression(ast::Expression* e);
    std::string toString(ast::Physical* p);
    std::string toString(ast::Number* i);
    std::string toString(ast::BasicIdentifier* i);
    void basicIdentifier(ast::BasicIdentifier* i);
    std::string toString(const char* separator, ast::BasicIdentifierList* list);
    void basicIdentifierList(const char* separator, ast::BasicIdentifierList* list);

    void sequentialStatements(ast::List<ast::SequentialStatement>& l);
    void waitStatement(ast::WaitStatement* p);
    void ifStatement(ast::IfStatement* p);
    void forLoopStatement(ast::ForLoopStatement* p);
    void reportStatement(ast::ReportStatement* p);
    void procedureCallStatement(ast::ProcedureCallStatement* p);
    void variableAssignment(ast::VariableAssignment* p);
    void signalAssignment(ast::SignalAssignment* p);
    void includes(int indent, ast::DesignFile& designFile);

    void numberType(ast::BasicIdentifier* identifier, ast::NumberType* t);
    void enumerationType(ast::BasicIdentifier* identifier, ast::EnumerationType* t);
    void type_declarations(ast::TypeDeclaration* t);

    void variable_declarations(ast::VariableDeclaration* v);
    void signal_declarations(ast::SignalDeclaration* v);
    void constant_declarations(ast::ConstantDeclaration* v);
    void declarations(ast::List<ast::Declaration>& d);

    void implementation(int indent, ast::DesignFile& designFile, ast::BasicIdentifier* name);

    void printSourceLine(ast::Text& t);
    void printSourceLine(ast::BasicIdentifier* t);
  public:
    SystemC(ast::DesignFile& designFile);
  };
}

#endif


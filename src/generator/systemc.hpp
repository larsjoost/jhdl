#ifndef GENERATOR_SYSTEMC_HPP_
#define GENERATOR_SYSTEMC_HPP_

#include <list>
#include <string>

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
#include "../ast/block_statement.hpp"
#include "../ast/wait_statement.hpp"
#include "../ast/sequential_statement.hpp"
#include "../ast/concurrent_statement.hpp"

namespace generator {
  
  class SystemC { 

    struct parameters {
      struct SignalDeclaration {
        std::string type;
        std::string id;
      };
      int indent;
      std::string parentName;
      std::list<std::string> forGenerateHierarchy;
      std::list<SignalDeclaration> signalDeclaration;
      void incIndent() {
        indent += 2;
      }
      void decIndent() {
        indent -= 2;
      }
      parameters(const parameters& p) {
        indent = p.indent;
        incIndent();
        forGenerateHierarchy = p.forGenerateHierarchy;
        parentName = p.parentName;
        signalDeclaration = p.signalDeclaration;
      }
      parameters() {
        indent = 0;
      }
    };
    
    void println(parameters& parm, std::string text);
  
    int methodId = 0;

    std::string toString(ast::ExpressionTerm& e);
    std::string toString(ast::Expression* e);
    void expression(ast::Expression* e);
    std::string toString(ast::Physical* p);
    std::string toString(ast::Number* i);
    std::string toString(ast::BasicIdentifier* i);
    void basicIdentifier(ast::BasicIdentifier* i);
    std::string toString(ast::BasicIdentifier* i, ast::RangeType* r);
    std::string toString(const char* separator, ast::BasicIdentifierList* list);
    void basicIdentifierList(const char* separator, ast::BasicIdentifierList* list);

    void sequentialStatements(parameters parm, ast::List<ast::SequentialStatement>& l);
    void waitStatement(parameters& parm, ast::WaitStatement* p);
    void ifStatement(parameters& parm, ast::IfStatement* p);
    void forLoopStatement(parameters& parm, ast::ForLoopStatement* p);
    void reportStatement(parameters& parm, ast::ReportStatement* p);
    void procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p);
    void variableAssignment(parameters& parm, ast::VariableAssignment* p);
    void signalAssignment(parameters& parm, ast::SignalAssignment* p);
    void includes(parameters& parm, ast::DesignFile& designFile);

    void numberType(parameters parm, ast::BasicIdentifier* identifier, ast::NumberType* t);
    void enumerationType(parameters parm, ast::BasicIdentifier* identifier, ast::EnumerationType* t);
    void type_declarations(parameters& parm, ast::TypeDeclaration* t);
    void variable_declarations(parameters& parm, ast::VariableDeclaration* v);
    void signal_declarations(parameters& parm, ast::SignalDeclaration* v);
    void constant_declarations(parameters& parm, ast::ConstantDeclaration* v);
    void declarations(parameters& parm, ast::List<ast::Declaration>& d);
    template<class T, typename Func>
    std::string toString(std::list<T>& t, std::string delimiter, Func lambda);
    void methodDefinition(parameters& parm, ast::Method* method);
    void methodInstantiation(parameters& parm, ast::Method* method);
    void blockStatementDefinition(parameters& parm, ast::BlockStatement* blockStatement);
    void blockStatementInstantiation(parameters parm, ast::BlockStatement* blockStatement);
    void forGenerateStatementDefinition(parameters parm, ast::ForGenerateStatement* forGenerateStatement);
    void forGenerateStatementInstantiation(parameters parm, ast::ForGenerateStatement* forGenerateStatement);
    void concurrentStatementsDefinition(parameters parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void concurrentStatementsInstantiation(parameters parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
  
    void implementation(parameters& parm, ast::DesignFile& designFile, ast::BasicIdentifier* name);

    void printSourceLine(parameters& parm, ast::Text& t);
    void printSourceLine(parameters& parm, ast::BasicIdentifier* t);
  public:
    SystemC(ast::DesignFile& designFile);
  };
}

#endif



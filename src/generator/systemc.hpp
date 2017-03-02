#ifndef GENERATOR_SYSTEMC_HPP_
#define GENERATOR_SYSTEMC_HPP_

#include <list>
#include <unordered_map>
#include <string>

#include "../ast/text.hpp"
#include "../ast/design_file.hpp"
#include "../ast/expression_term.hpp"
#include "../ast/expression.hpp"
#include "../ast/procedure_call_statement.hpp"
#include "../ast/variable_assignment.hpp"
#include "../ast/return_statement.hpp"
#include "../ast/object_declaration.hpp"
#include "../ast/enumeration_type.hpp"
#include "../ast/number_type.hpp"
#include "../ast/character.hpp"
#include "../ast/basic_identifier.hpp"
#include "../ast/basic_identifier_list.hpp"
#include "../ast/report_statement.hpp"
#include "../ast/if_statement.hpp"
#include "../ast/forloop_statement.hpp"
#include "../ast/block_statement.hpp"
#include "../ast/wait_statement.hpp"
#include "../ast/sequential_statement.hpp"
#include "../ast/concurrent_statement.hpp"
#include "../ast/interface_element.hpp"
#include "../ast/interface_list.hpp"
#include "../ast/interface.hpp"

namespace generator {
  
  class SystemC { 

    bool verbose = false;

    void functionStart(std::string name);
    void functionEnd(std::string name);

    enum DeclarationID {SIGNAL, FUNCTION};
    
    struct DeclarationInfo {
      DeclarationID id;
      int hierarchyLevel = 0;
    };
    
    struct parameters {
      int indent = 0;
      std::string parentName;
      std::list<std::string> forGenerateHierarchy;
      std::unordered_map<std::string, DeclarationInfo> declaration;
      void incIndent() {
        indent += 2;
      }
      void decIndent() {
        indent -= 2;
      }
    };
    
    void printDeclaration(parameters& parm);
    
    void println(parameters& parm, std::string text);
  
    int methodId = 0;

    bool matchDeclarationID(parameters& parm, std::string& name, DeclarationID id);
    int getHierarchyLevel(parameters& parm, std::string& name);

    parameters descendHierarchy(parameters& parm);
    
    std::string expressionTermToString(parameters& parm, ast::ExpressionTerm* e);
    std::string expressionToString(parameters& parm, ast::Expression* e);
    std::string physicalToString(parameters& parm, ast::Physical* p);
    std::string numberToString(parameters& parm, ast::Number* i);
    std::string characterToString(parameters& parm, ast::Character* i);
    std::string basicIdentifierToString(parameters& parm, ast::BasicIdentifier* i);
    std::string rangeTypeToString(parameters& parm, ast::BasicIdentifier* i, ast::RangeType* r);

    void sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l);
    void waitStatement(parameters& parm, ast::WaitStatement* p);
    void ifStatement(parameters& parm, ast::IfStatement* p);
    void forLoopStatement(parameters& parm, ast::ForLoopStatement* p);
    void reportStatement(parameters& parm, ast::ReportStatement* p);
    std::string procedureCallStatementToString(parameters& parm, ast::ProcedureCallStatement* p);
    void procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p);
    void returnStatement(parameters& parm, ast::ReturnStatement* r);
    void variableAssignment(parameters& parm, ast::VariableAssignment* p);
    void signalAssignment(parameters& parm, ast::SignalAssignment* p);
    void includes(parameters& parm, ast::DesignFile& designFile);

    void numberType(parameters& parm, ast::BasicIdentifier* identifier, ast::NumberType* t);
    void enumerationType(parameters& parm, ast::BasicIdentifier* identifier, ast::EnumerationType* t);
    void type_declarations(parameters& parm, ast::TypeDeclaration* t);
    std::string objectDeclarationToString(parameters& parm, ast::ObjectDeclaration* v);
    void object_declarations(parameters& parm, ast::ObjectDeclaration* v);
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter = ", ");
    void function_declarations(parameters& parm, ast::FunctionDeclaration* f);
    void declarations(parameters& parm, ast::List<ast::Declaration>& d);

    template<class Key, class Value, typename Func>
    std::string listToString(parameters& parm, std::unordered_map<Key, Value>& t, std::string delimiter, Func lambda);
    template<class T, typename Func>
    std::string listToString(parameters& parm, std::list<T>& t, std::string delimiter, Func lambda);
    template<typename Func>
    std::string listToString(parameters& parm, ast::BasicIdentifierList* list, std::string delimiter, Func lambda);

    std::string getConstructorDeclaration(parameters& parm, std::string& name);
    void methodDefinition(parameters& parm, ast::Method* method);
    void instantiateType(parameters& parm, std::string type, std::string name);
    void methodInstantiation(parameters& parm, ast::Method* method);
    void blockStatementDefinition(parameters& parm, ast::BlockStatement* blockStatement);
    void blockStatementInstantiation(parameters& parm, ast::BlockStatement* blockStatement);
    void forGenerateStatementDefinition(parameters& parm, ast::ForGenerateStatement* forGenerateStatement);
    void forGenerateStatementInstantiation(parameters& parm, ast::ForGenerateStatement* forGenerateStatement);
    void concurrentStatementsDefinition(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void concurrentStatementsInstantiation(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
  
    void threadConstructor(parameters& parm, ast::BasicIdentifier* name, 
                           ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void interface(parameters& parm, ast::Interface* intf);
    void implementation(parameters& parm, ast::DesignFile& designFile, ast::BasicIdentifier* name);

    void printSourceLine(parameters& parm, ast::Text& t);
    void printSourceLine(parameters& parm, ast::BasicIdentifier* t);
  public:
    SystemC(bool verbose = false);
    void generate(ast::DesignFile& designFile);
  };
}

#endif



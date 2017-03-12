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
#include "../ast/array_type.hpp"
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
#include "../ast/component_instance.hpp"
#include "../ast/subtype_indication.hpp"
#include "../ast/subtype_declaration.hpp"

namespace generator {
  
  class SystemC { 

    bool verbose = false;

    void functionStart(std::string name);
    void functionEnd(std::string name);

    enum DeclarationID {SIGNAL, VARIABLE, CONSTANT, FUNCTION, PORT};

    struct DeclarationInfo {
      DeclarationID id;
      int hierarchyLevel = 0;
    };
    
    struct parameters {
      int indent = 0;
      bool definesAllowed = true;
      bool instanceAllowed = true;
      std::string parentName;
      std::list<std::string> forGenerateHierarchy;
      std::unordered_map<std::string, DeclarationInfo> declaration;
      std::unordered_map<std::string, ast::FunctionDeclaration*> functions;
      void incIndent() {
        indent += 2;
      }
      void decIndent() {
        indent -= 2;
      }
    };
    
    void printDeclaration(parameters& parm);
    
    void println(parameters& parm, std::string text);

    void printError(ast::Text& t, std::string message);
    void printWarning(ast::Text& t, std::string message);

    int methodId = 0;

    bool matchDeclarationID(parameters& parm, std::string& name, DeclarationID id);
    int getHierarchyLevel(parameters& parm, std::string& name);

    parameters descendHierarchy(parameters& parm);

    template <typename Func>
    std::string expressionTermToString(parameters& parm, ast::ExpressionTerm* e, Func basicIdentifierCallback);
    template <typename Func>
    std::string expressionToString(parameters& parm, ast::Expression* e, Func basicIdentifierCallback);
    std::string expressionToString(parameters& parm, ast::Expression* e);
    std::string physicalToString(parameters& parm, ast::Physical* p);
    std::string numberToString(parameters& parm, ast::Number* i);
    std::string characterToString(parameters& parm, ast::Character* i);
    std::string getName(parameters& parm, ast::BasicIdentifier* i, bool hierarchy = false);
    std::string basicIdentifierToString(parameters& parm, ast::BasicIdentifier* i);
    std::string rangeStruct(std::string& name, std::string& left, std::string& right);

    void sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l);
    void waitStatement(parameters& parm, ast::WaitStatement* p);
    void ifStatement(parameters& parm, ast::IfStatement* p);
    template<typename Func>
    void forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback);
    void forLoopStatement(parameters& parm, ast::ForLoopStatement* p);
    void reportStatement(parameters& parm, ast::ReportStatement* p);
    std::string procedureCallStatementToString(parameters& parm, ast::ProcedureCallStatement* p);
    std::string parametersToString(parameters& parm, ast::BasicIdentifier* functionName, ast::AssociationList* l = NULL);
    std::string associateArgument(parameters& parm, std::string& name, std::string& init, int argumentNumber, ast::AssociationList* l);
    void procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p);
    void returnStatement(parameters& parm, ast::ReturnStatement* r);
    void variableAssignment(parameters& parm, ast::VariableAssignment* p);
    void signalAssignment(parameters& parm, ast::SignalAssignment* p);
    void includes(parameters& parm, ast::DesignFile& designFile);

    void numberType(parameters& parm, ast::BasicIdentifier* identifier, ast::NumberType* t);
    void enumerationType(parameters& parm, ast::BasicIdentifier* identifier, ast::EnumerationType* t);
    void arrayType(parameters& parm, ast::BasicIdentifier* identifier, ast::ArrayType* t);
    void type_declarations(parameters& parm, ast::TypeDeclaration* t);
    void printArrayType(parameters& parm, std::string& name, ast::RangeType* r, std::string& subtype);
    void printRangeType(parameters& parm, std::string& name, ast::RangeType* r);
    void printSubtype(parameters& parm, std::string& name, ast::RangeType* r, std::string typeName);
    std::string subtypeIndication(parameters& parm, std::string& name, ast::SubtypeIndication* t);
    void subtype_declarations(parameters& parm, ast::SubtypeDeclaration* t);
    void subtypeIndicationToString(parameters& parm, ast::SubtypeIndication* s,
                                   std::string& name, std::string& type,
                                   std::string& preDefinition);
    template<typename Func>
    void objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback);
    std::string objectDeclarationToString(parameters& parm, ast::ObjectDeclaration* v,
                                          bool initialization);
    void object_declarations(parameters& parm, ast::ObjectDeclaration* v);
    template<typename Func>
    void traverseInterfaceList(parameters& parm, ast::InterfaceList* l, Func callback);
    template<typename Func>
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                      bool initialization, Func typeConverter);
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                      bool initialization);
    void function_declarations(parameters& parm, ast::FunctionDeclaration* f);
    void declarations(parameters& parm, ast::List<ast::Declaration>& d);

    template<class Key, class Value, typename Func>
    std::string listToString(parameters& parm, std::unordered_map<Key, Value>& t, std::string delimiter, Func callback);
    template<class T, typename Func>
    std::string listToString(parameters& parm, std::list<T>& t, std::string delimiter, Func callback);
    template<class T, typename Func>
    std::string listToString(parameters& parm, std::list<T>* t, std::string delimiter, Func callback);
    template<typename Func>
    std::string listToString(parameters& parm, ast::BasicIdentifierList* list, std::string delimiter, Func callback);
    template<typename Func, class T>
    std::string listToString(parameters& parm, ast::List<T>* list, std::string delimiter, Func callback);

    std::string getConstructorDeclaration(parameters& parm, std::string& name);
    template <class T, typename DeclarationFunc, typename BodyFunc>
    void scThreadShell(parameters& parm, std::string& name, T sensitivity, DeclarationFunc declarations, BodyFunc body); 
    void methodDefinition(parameters& parm, ast::Method* method);
    void instantiateType(parameters& parm, std::string type, std::string name);
    void signalInstantiation(parameters& parm, ast::SignalAssignment* s);
    void methodInstantiation(parameters& parm, ast::Method* method);
    void blockStatementDefinition(parameters& parm, ast::BlockStatement* blockStatement);
    void blockStatementInstantiation(parameters& parm, ast::BlockStatement* blockStatement);
    void forGenerateStatementDefinition(parameters& parm, ast::ForGenerateStatement* forGenerateStatement);
    void forGenerateStatementInstantiation(parameters& parm, ast::ForGenerateStatement* forGenerateStatement);
    void componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l);
    void componentInstantiation(parameters& parm, ast::ComponentInstance* c);
    void concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s);
    void concurrentStatementsDefinition(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void concurrentStatementsInstantiation(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
  
    void threadConstructor(parameters& parm, ast::BasicIdentifier* name, 
                           ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void interface(parameters& parm, ast::Interface* intf);
    void implementation(parameters& parm, ast::DesignFile& designFile, ast::Interface* interface);

    void printSourceLine(parameters& parm, ast::Text& t);
    void printSourceLine(parameters& parm, ast::BasicIdentifier* t);
  public:
    SystemC(bool verbose = false);
    void generate(ast::DesignFile& designFile);
  };
}

#endif



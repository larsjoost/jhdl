#ifndef GENERATOR_SYSTEMC_HPP_
#define GENERATOR_SYSTEMC_HPP_

#include <list>
#include <unordered_map>
#include <string>
#include <cassert>
#include <fstream>

#include "config.hpp"
#include "database/database.hpp"
#include "expression/expression.hpp"

#include "../exceptions/exceptions.hpp"
#include "../debug/debug.hpp"

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
#include "../ast/function_declaration.hpp"
#include "../ast/function_body.hpp"
#include "../ast/association_list.hpp"
#include "../ast/association_element.hpp"

#include "parameters.hpp"
#include "name_converter.hpp"

namespace generator {
  
  class SystemC { 

    Exceptions exceptions;
    Debug debug = Debug("SystemC", false);

    const std::string libraryInfoFilename = ".jhdl.ini";
    
    bool verbose = false;

    std::string filename;

    parameters headerParameters;
    parameters sourceParameters;
    
    Database a_database;
    NameConverter a_name_converter = NameConverter(a_database);
    ExpressionParser a_expression = ExpressionParser(a_database, a_name_converter); 
    
    Config config;
    Config libraryInfo;

    int methodId = 0;

    void namespaceStart(parameters& parm, std::string& library);
    void namespaceEnd(parameters& parm);
    
    // general.cpp
    void topHierarchyStart(parameters& parm, std::string& library, std::string& name, ast::ObjectType type, std::string& filename);
    void topHierarchyEnd(parameters& parm, bool globalize);
    void descendHierarchy(parameters& parm, std::string parentName = "");
    void ascendHierarchy(parameters& parm);

    DatabaseElement* getName(parameters& parm, ast::BasicIdentifier* i, std::string& name);
    std::string rangeStruct(std::string& name, std::string& left, std::string& right);

    // sequential.cpp
    void signalAssignment(parameters& parm, ast::SignalAssignment* p);
    template<typename Func>
    void signalAssignment(parameters& parm, ast::SignalAssignment* p, Func callback);
    void sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l);
    void waitStatement(parameters& parm, ast::WaitStatement* p);
    void ifStatement(parameters& parm, ast::IfStatement* p);
    template<typename Func>
    void forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback);
    void forLoopStatement(parameters& parm, ast::ForLoopStatement* p);
    void reportStatement(parameters& parm, ast::ReportStatement* p);
    std::string getArgumentNames(parameters& parm, ast::AssociationList* arguments);
    std::string associateArgument(parameters& parm, std::string& name, std::string& init, int argumentNumber, ast::AssociationList* l);
    void procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p);
    void returnStatement(parameters& parm, ast::ReturnStatement* r);
    void variableAssignment(parameters& parm, ast::VariableAssignment* p);

    // includes.cpp
    void loadPackage(std::string package, std::string library,
                     std::string identifier, ast::Text* text = NULL);
    void includes(parameters& parm, ast::ContextClause* contextClause, bool load);

    ast::ObjectValueContainer numberType(parameters& parm, ast::SimpleIdentifier* identifier, ast::NumberType* t);
    ast::ObjectValueContainer enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t);
    ast::ObjectValueContainer arrayType(parameters& parm, ast::SimpleIdentifier* identifier, ast::ArrayType* t);
    void printArrayType(parameters& parm, std::string& name, ast::ArrayDefinition* r, std::string& subtype);
    void rangeToString(ast::RangeType* r, std::string& left, std::string& right, ast::ObjectValueContainer& type);
    void printRangeType(parameters& parm, std::string& name, ast::RangeType* r);
    void printPhysicalType(parameters& parm, std::string& name, ast::NumberType* n);
    void printSubtype(parameters& parm, std::string& name, ast::RangeType* r, std::string typeName, ast::ObjectValueContainer& type);
    std::string subtypeIndication(parameters& parm, DatabaseResult& database_result, std::string& name,
                                  ast::SubtypeIndication* t);
    void subtypeIndicationToString(parameters& parm, ast::SubtypeIndication* s,
                                   std::string& name, std::string& type,
                                   std::string& preDefinition);

    // declarations.hpp
    template<typename Func>
    void objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback);
    std::string objectDeclarationToString(parameters& parm, ast::ObjectDeclaration* v,
                                          bool initialization);
    void object_declarations(parameters& parm, ast::ObjectDeclaration* v);
    template<typename Func>
    void traverseInterfaceList(parameters& parm, ast::InterfaceList* l, bool printEnable, Func callback);
    template<typename Func>
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                      bool initialization, Func typeConverter);
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                      bool initialization);
    
    // declarations.cpp
    void subtype_declarations(parameters& parm, ast::SubtypeDeclaration* t);
    void type_declarations(parameters& parm, ast::TypeDeclaration* t);
    std::string getInterface(parameters& parm, ast::InterfaceList* interface);
    std::string getArgumentTypes(parameters& parm, ast::InterfaceList* interface);
    std::string getArgumentNames(parameters& parm, ast::InterfaceList* interface);
    std::string getArgumentTypes(parameters& parm, ast::List<ast::SimpleIdentifier>* arguments);
    bool findType(ast::SimpleIdentifier* name, ast::ObjectValueContainer& type);
    void generateObjectArguments(ast::List<ast::SimpleIdentifier>* args, ast::ObjectArguments& arguments);
    void generateObjectArguments(ast::InterfaceList* interface, ast::ObjectArguments& arguments);
    std::string function_attribute(parameters& parm, std::string& name, ast::ObjectType type,
                                   std::string& interface, ast::ObjectArguments& arguments,
                                   std::string returnType, ast::Text* text);
    std::string function_attribute(parameters& parm, DatabaseElement* e, std::string& interface);
    void function_declarations(parameters& parm, ast::FunctionDeclaration* f, bool implementation);
    void FunctionBody(parameters& parm, ast::List<ast::Declaration>& d,
                      ast::List<ast::SequentialStatement>& s);
    void procedure_declarations(parameters& parm, ast::ProcedureDeclaration* f, bool implementation);
    void attribute_declarations(parameters& parm, ast::Attribute* a);
    void declarations(parameters& parm, ast::List<ast::Declaration>& d, bool implementation = false);

    // definition.cpp
    template <typename Func>
    void createProcess(parameters& parm, Func func);
    template <typename Func>
    void printSensitivityListWait(parameters& parm, auto sensitivity, Func func);
    template <class T, typename Func>
    void createThread(parameters& parm, std::string& name, T sensitivity,
                      ast::List<ast::Declaration>* declarationList,
                      Func body);  
    std::string getConstructorDeclaration(parameters& parm, std::string& name, std::string* argument);
    bool getObjectName(std::string& name, ast::ObjectValueContainer& type, ast::ObjectType id, ast::Text* text = NULL);
    bool getObjectName(std::string& name, ast::ObjectType id, ast::Text* text = NULL);
    template <typename BodyFunc, typename DeclFunc>
    void defineObject(parameters& parm,
                      bool topHierarchy,
                      std::string name,
                      std::string type,
		      std::string* argument,
                      ast::List<ast::Declaration>* declarations,
                      ast::List<ast::ConcurrentStatement>* concurrentStatements,
                      BodyFunc bodyCallback,
		      DeclFunc declarationCallback,
                      std::string* constructor = NULL);
    void forGenerateStatementDefinition(parameters& parm, ast::ForGenerateStatement* forGenerateStatement);
    void blockStatementDefinition(parameters& parm, ast::BlockStatement* blockStatement);
    void concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s);
    void methodDefinition(parameters& parm, ast::Method* method);

    
    template<class Key, class Value, typename Func>
    std::string listToString(parameters& parm, std::unordered_map<Key, Value>& t, std::string delimiter, Func callback);
    template<class T, typename Func>
    std::string listToString(parameters& parm, std::list<T>& t, std::string delimiter, Func callback);
    template<class T, typename Func>
    std::string listToString(parameters& parm, std::list<T>* t, std::string delimiter, Func callback);
    template<typename Func, class T>
    std::string listToString(parameters& parm, ast::List<T>* list, std::string delimiter, Func callback);

    void concurrentStatementsDefinition(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);

    // constructor.cpp
    void instantiateType(parameters& parm, std::string type, std::string name, std::string arguments = "");
    void signalInstantiation(parameters& parm, ast::SignalAssignment* s);
    void methodInstantiation(parameters& parm, ast::Method* method);
    void blockStatementInstantiation(parameters& parm, ast::BlockStatement* blockStatement);
    void forGenerateStatementInstantiation(parameters& parm, ast::ForGenerateStatement* forGenerateStatement);
    void concurrentStatementsInstantiation(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l,
                              std::string& entityName, std::string& library);
    void componentInstantiation(parameters& parm, ast::ComponentInstance* c);
    void createConstructor(parameters& parm, bool toopHierarchy, std::string& name,
                           std::string* argument,
                           ast::List<ast::ConcurrentStatement>* concurrentStatements,
                           std::string* constructor);

    // systemc.cpp
    void packageDeclaration(parameters& parm, ast::Package* package, std::string& library);
    void interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library);
    void implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library);

    void printSourceLine(parameters& parm, ast::Text* t);
    void printSourceLine(parameters& parm, ast::Text& t);
    void printSourceLine(parameters& parm, ast::BasicIdentifier* t);
    void printSourceLine(parameters& parm, ast::SimpleIdentifier* t);

    void parse(parameters& parm, ast::DesignFile& designFile, std::string& library);
    void parsePackage(parameters& parm, std::string name, std::string library);

  public:
    SystemC(bool verbose = false);
    void generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename);
    void saveLibraryInfo();
  };

  template<class Key, class Value, typename Func>
  std::string SystemC::listToString(parameters& parm, std::unordered_map<Key, Value>& t,
                                    std::string delimiter, Func callback) {
    std::string s;
    std::string d;
    for (auto x : t) {
      std::string c = callback(x.first, x.second);
      if (c.size() > 0) {
        s += (d + c);
        d = delimiter;
      }
    }
    return s;
  }

  template<class T, typename Func>
  std::string SystemC::listToString(parameters& parm, std::list<T>& t,
                                    std::string delimiter, Func callback) {
    return listToString(parm, &t, delimiter, callback);
  }
  
  template<class T, typename Func>
  std::string SystemC::listToString(parameters& parm, std::list<T>* t,
                                    std::string delimiter, Func callback) {
    std::string s;
    std::string d;
    for (auto x : *t) {
      std::string c = callback(x);
      if (c.size() > 0) {
        s += (d + c);
        d = delimiter;
      }
    }
    return s;
  }

  template<typename Func, class T>
  std::string SystemC::listToString(parameters& parm, ast::List<T>* list,
                                    std::string delimiter, Func callback) {
    std::string s = "";
    if (list) {
      std::string d = "";
      for (auto t : list->list) {
        std::string c = callback(t);
        if (c.size() > 0) {
          s += (d + c);
          d = delimiter;
        }
      }
    }
    return s;
  }

  template <typename Func>
  std::string SystemC::interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                             bool initialization, Func typeConverter) {
    std::string s;
    if (l) {
      std::string x = "";
      std::string d = "";
      traverseInterfaceList(parm, l, true,
                            [&](std::string& name,
                                std::string& type, std::string& init,
                                ast::ObjectType id, ast::ObjectDeclaration::Direction direction) {
                              std::string x = (initialization && !init.empty()) ? " = " + init : "";
                              s += d + typeConverter(type, id, direction) + " " + name + x;
          d = delimiter;
        }
        );
    }
    return s;
  }

  template<typename Func>
  void SystemC::traverseInterfaceList(parameters& parm, ast::InterfaceList* l, bool printEnable, Func callback) {
    debug.functionStart("traverseInterfaceList");
    if (l) {
      bool q = parm.setQuiet(!printEnable);
      for (ast::InterfaceElement i : l->interfaceElements.list) {
        if (i.object) {
          objectDeclaration(parm, i.object, callback);
        }
      }
      parm.setQuiet(q);
    }
    debug.functionEnd("traverseInterfaceList");
  }



}
#endif



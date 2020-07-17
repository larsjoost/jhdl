#ifndef GENERATOR_SYSTEMC_HPP_
#define GENERATOR_SYSTEMC_HPP_

#include <list>
#include <unordered_map>
#include <string>
#include <cassert>
#include <fstream>

#include "config.hpp"
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

    bool m_convert_to_systemc = false;
    
    Debug<false> m_debug;

    Exceptions exceptions;
    
    const std::string libraryInfoFilename = ".jhdl.ini";
    
    bool a_verbose = false;

    std::string a_filename;

    ExpressionParser a_expression; 

    Config config;
    Config libraryInfo;

    struct RangeDefinition {
      std::string left;
      std::string right;
      std::string ascending;
    };
    
    // general.cpp
    void topHierarchyStart(parameters& parm, std::string& library, std::string& name, ast::ObjectType type, std::string& filename);
    void topHierarchyEnd(parameters& parm, ast::Text* text, bool globalize = false);
    void openHierarchy(parameters& parm, std::string name, ast::ObjectType type , std::string class_description, std::string base_name);
    void closeHierarchy(parameters& parm);

    DatabaseElement* getName(parameters& parm, ast::BasicIdentifier* i, std::string& name);
    std::string rangeStruct(std::string& name, std::string& left, std::string& right);

    // sequential.cpp
    template<typename Func>
    void assignment(parameters& parm, ast::Assignment* p, ast::BasicIdentifier* target, ast::ObjectType object_type, std::list<std::string>& sequential_list,
		    Func sensitivity_list_sensitivity_list_callback, bool signal_assignment);
    template<typename Func>
    void signalAssignment(parameters& parm, ast::SignalAssignment* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback);
    template<typename Func>
    void sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l, std::list<std::string>& sequential_list, Func sensitivity_list_callback);
    void waitStatement(parameters& parm, ast::WaitStatement* p, std::list<std::string>& sequential_list);
    template<typename Func>
    void ifStatement(parameters& parm, ast::IfStatement* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback);
    template<typename Func>
    void forLoop(parameters& parm, std::string& name, ast::IterationScheme* iteration, Func callback);
    template<typename Func>
    void forLoopStatement(parameters& parm, ast::ForLoopStatement* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback);
    bool lookupSeverity(parameters& parm, ast::ReportStatement* p, std::string& severity, std::list<std::string>& sequential_list);
    void reportStatement(parameters& parm, ast::ReportStatement* p, std::list<std::string>& sequential_list);
    std::string getArgumentNames(parameters& parm, ast::AssociationList* arguments);
    std::string associateArgument(parameters& parm, std::string& name, std::string& init, int argumentNumber, ast::AssociationList* l);
    void procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p, std::list<std::string>& sequential_list);
    void returnStatement(parameters& parm, ast::ReturnStatement* r, std::list<std::string>& sequential_list);
    void variableAssignment(parameters& parm, ast::VariableAssignment* p, std::list<std::string>& sequential_list);

    // includes.cpp
    void loadPackage(parameters& parm, std::string package, std::string library,
                     std::string identifier, ast::Text* text = NULL);
    void includes(parameters& parm, ast::ContextClause* contextClause, bool load);

    ast::ObjectValueContainer SimpleType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type,
                                         ast::ObjectValue object_value, std::string definition);
    ast::ObjectValueContainer FileType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type);
    ast::ObjectValueContainer AccessType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type);
    ast::ObjectValueContainer enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t);
    template<typename Func>
    void PrintTypeObject(parameters& parm, const std::string& name, Func func);
    template<typename Func>
    void printFactoryDefinition(parameters& parm, const std::string& name, Func func,
				const std::string subtype_name = "");
    void printArrayType(parameters& parm, std::string& name, ast::List<ast::ArrayDefinition>& definition, std::string& subtype_name, ast::RangeType* subtype_range,
                        ast::ObjectValueContainer::Array& arguments);
    ast::ObjectValue printRangeType(parameters& parm, std::string& name, ast::RangeType* r);
    void printPhysicalType(parameters& parm, std::string& name, ast::NumberType* n);

    // declarations.hpp
    template<typename Func>
    void ObjectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback, std::string local_prefix = "",
                           bool database_enable = true);
    void ObjectDeclarations(parameters& parm, ast::ObjectDeclaration* v);
    template<typename Func>
    void traverseInterfaceList(parameters& parm, ast::InterfaceList* l, Func callback, std::string local_prefix = "",
                               bool database_enable = true);
    template<typename Func>
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                      bool initialization, Func typeConverter, std::string local_prefix = "",
                                      bool database_enable = true);
    std::string interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                      bool initialization, std::string local_prefix = "",
                                      bool database_enable = true);
    
    // declarations.cpp
    std::string getSignalType(std::string& type);
    void printFactory(parameters& parm, const std::string& name, 
                      ast::RangeType* range, ast::SimpleIdentifier* identifier,
                      ast::ObjectValueContainer& expected_type,
                      ast::ArraySubtypeDefinition* subtype = NULL);
    ast::ObjectValueContainer arrayType(parameters& parm, ast::SimpleIdentifier* identifier, ast::ArrayType* t);
    void subtype_declarations(parameters& parm, ast::SubtypeDeclaration* t);
    ast::ObjectValueContainer numberType(parameters& parm, ast::SimpleIdentifier* identifier, ast::NumberType* t);
    void type_declarations(parameters& parm, ast::TypeDeclaration* t);
    void FileDeclaration(parameters& parm, ast::FileDeclaration* file);
    void AliasDeclaration(parameters& parm, ast::AliasDeclaration* file);
    std::string InterfaceTypeConverter(std::string& type, ast::ObjectType id,
                                       ast::ObjectDeclaration::Direction direction);
    void StoreInterfaceInDatabase(parameters& parm, ast::InterfaceList* interface);
    void PrintInterface(parameters& parm, ast::InterfaceList* interface, bool database_enable = true);
    std::string GetInterface(parameters& parm, ast::InterfaceList* interface,
                             bool initialization = true, std::string local_prefix = "");
    std::string getArgumentTypes(parameters& parm, ast::InterfaceList* interface);
    std::string getArgumentNames(parameters& parm, ast::InterfaceList* interface);
    std::string getArgumentTypes(parameters& parm, ast::List<ast::SimpleIdentifier>* arguments);
    void generateObjectInterface(parameters& parm, ast::List<ast::SimpleIdentifier>* args, ast::ObjectInterface& interface);
    void generateObjectInterface(parameters& parm, ast::InterfaceList* interface_list,
				 ast::ObjectInterface& interface);
    std::string functionAttribute(parameters& parm, std::string& name, ast::ObjectType type,
                                  ast::ObjectInterface& interface, ast::Text* text);
    std::string function_attribute(parameters& parm, DatabaseElement* e, std::string& interface);
    std::string FunctionReturn(parameters& parm, ast::FunctionDeclaration* f, bool global_scope);
    void function_declarations(parameters& parm, ast::FunctionDeclaration* f);
    std::string AttributeName(ast::Attribute* a);
    void ForeignAttribute(parameters& parm, ast::Attribute* a);
    void attribute_declarations(parameters& parm, ast::Attribute* a);
    void declarations(parameters& parm, ast::List<ast::Declaration>& d);

    // definition.cpp
    void createProcess(parameters& parm, ast::SimpleIdentifier* label, ast::Text* line,
		       std::list<std::string>* sensitivity_list,
		       bool sensitivity_all,
		       ast::List<ast::Declaration>* declarations,
		       ast::List<ast::SequentialStatement>& sequential_statements);
    template <class T, typename Func>
    void createThread(parameters& parm, std::string& name, T sensitivity,
                      ast::List<ast::Declaration>* declarationList,
                      Func body);  
    bool getObjectName(parameters& parm, std::string& name, ast::ObjectValueContainer& type,
		       ast::ObjectType id, ast::Text* text = NULL);
    bool getObjectName(parameters& parm, std::string& name, ast::ObjectType id,
		       ast::Text* text = NULL);
    template <typename F1, typename F2>
    void defineObject(parameters& parm,
                      bool topHierarchy,
                      std::string name,
		      ast::ObjectType type,
		      std::string base_name,
		      std::string* class_description,
		      std::string* argument,
                      ast::List<ast::Declaration>* declarations,
                      ast::List<ast::ConcurrentStatement>* concurrentStatements,
                      F1 initial_callback,
                      F2 body_callback,
                      bool wait_statements,
                      bool init_enable);
    void forGenerateStatementDefinition(parameters& parm,
					ast::ForGenerateStatement* forGenerateStatement);
    void blockStatementDefinition(parameters& parm, ast::BlockStatement* blockStatement);
    void concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s);
    void processDefinition(parameters& parm, ast::Process* process);

    
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
    void instantiateType(parameters& parm, std::string name, ast::ObjectType object_type, std::list<std::string>* sensitivity_list = NULL, std::string description_append = "", std::string instance_argument = "");
    void concurrentStatementsInstantiation(parameters& parm, ast::List<ast::ConcurrentStatement>& concurrentStatements);
    void componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l,
                              std::string& entityName, std::string& library);
    void componentInstantiation(parameters& parm, ast::ComponentInstance* c);
    void createConstructor(parameters& parm, bool toopHierarchy, ast::ObjectType type,
                           std::string& name, std::string* argument,
                           ast::List<ast::ConcurrentStatement>* concurrentStatements);

    // systemc.cpp
    ast::ObjectValueContainer rangeAttributeToString(parameters& parm, ast::RangeAttributeType* r);
    ast::ObjectValueContainer rangeToString(parameters& parm, ast::RangeType* r, RangeDefinition& range_definition,
					    ast::ObjectValueContainer& type);
    std::string ObjectName(const ParentInfo& info);
    void packageDeclaration(parameters& parm, ast::Package* package, std::string& library);
    void interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library);
    void implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library);

    std::string getSourceLine(ast::Text* t);
    std::string getSourceLine(ast::Text& t);
    std::string getSourceLine(ast::BasicIdentifier* t);
    std::string getSourceLine(ast::SimpleIdentifier* t);
    
    void parse(parameters& parm, ast::DesignFile& designFile, std::string& library);
    void parsePackage(parameters& parm, std::string name, std::string library);

  public:
    SystemC(bool verbose = false);
    void generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename,
                  bool standardPackage);
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
                                             bool initialization, Func typeConverter, std::string local_prefix,
                                             bool database_enable) {
    m_debug.functionStart("interfaceListToString");
    std::string s;
    if (l) {
      std::string x = "";
      std::string d = "";
      auto func = [&](std::string& name, ast::ObjectValueContainer& type,
		      std::string& type_name, std::string& init, std::string& factory_name, 
                      ast::ObjectType id, ast::ObjectDeclaration::Direction direction) {
        std::string x = (initialization && !init.empty()) ? " = " + init : "";
	s += d + typeConverter(type_name, id, direction) + " " + name + x;
        d = delimiter;
      };
      traverseInterfaceList(parm, l, func, local_prefix, database_enable);
    }
    m_debug.functionEnd("interfaceListToString");
    return s;
  }

  template<typename Func>
  void SystemC::traverseInterfaceList(parameters& parm, ast::InterfaceList* l, Func callback,
                                      std::string local_prefix,
                                      bool database_enable) {
    m_debug.functionStart("traverseInterfaceList");
    if (l) {
      for (ast::InterfaceElement i : l->interfaceElements.list) {
        if (i.object) {
          ObjectDeclaration(parm, i.object, callback, local_prefix, database_enable);
        }
      }
    }
    m_debug.functionEnd("traverseInterfaceList");
  }



}
#endif



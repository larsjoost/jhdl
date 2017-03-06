#include <iostream>
#include <list>
#include <cassert>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"
#include "../ast/interface.hpp"
#include "../ast/implementation.hpp"
#include "../ast/method.hpp"
#include "../ast/context_clause.hpp"
#include "../ast/expression.hpp"
#include "../ast/expression_term.hpp"
#include "../ast/declaration.hpp"
#include "../ast/object_declaration.hpp"
#include "../ast/function_declaration.hpp"
#include "../ast/variable_assignment.hpp"
#include "../ast/signal_assignment.hpp"
#include "../ast/report_statement.hpp"
#include "../ast/component_instance.hpp"

#include "systemc.hpp"

namespace generator {

  void SystemC::println(parameters& parm, std::string text) {
    std::cout << std::string(parm.indent, ' ') << text << std::endl;
  }

  void SystemC::printError(ast::Text& t, std::string message) {
    t.printException("error", message);
  }
  
  void SystemC::printWarning(ast::Text& t, std::string message) {
    t.printException("warning", message, std::cout, "// ");
  }

  void SystemC::functionStart(std::string name) {
    if (verbose) {
      std::cerr << std::endl << "[FUNCTION START] " << name << std::endl;
    }
  }

  void SystemC::functionEnd(std::string name) {
    if (verbose) {
      std::cerr << std::endl << "[FUNCTION END] " << name << std::endl;
    }
  }

  SystemC::SystemC(bool verbose) : verbose(verbose) {}

  void SystemC::generate(ast::DesignFile& designFile) {
    functionStart("SystemC");
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    parameters parm;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.interface) {
        std::string name = basicIdentifierToString(parm, it->module.interface->name);
        parm.parentName = name;
        println(parm, "#include \"systemc.h\"");
        println(parm, "#include \"vhdl.h\"");
        println(parm, "namespace vhdl {");
        parm.incIndent();
	println(parm, "using namespace STANDARD;");
        includes(parm, designFile);
        println(parm, "");
        println(parm, "SC_MODULE(" + name + ") {");
        println(parm, "public:");
        parm.incIndent();
        interface(parm, it->module.interface);
        implementation(parm, designFile, it->module.interface);
        parm.decIndent();
        println(parm, "};");
        parm.decIndent();
        println(parm, "}");
      }
    }
  }

  void SystemC::printDeclaration(parameters& parm) {
    for (auto i : parm.declaration) {
      std::cout << "name = " << i.first << ", hierarchy level = " << i.second.hierarchyLevel << std::endl;
    }
  }
  
  SystemC::parameters SystemC::descendHierarchy(parameters& parm) {
    parameters p = parm;
    for (auto it = p.declaration.begin(); it != p.declaration.end(); it++) {
      it->second.hierarchyLevel++;
    }
    return p;
  }

  void SystemC::printSourceLine(parameters& parm, ast::Text& t) {
    println(parm, "// line " + std::to_string(t.getLine()) + ": " + t.getCurrentLine());
  }
  
  void SystemC::printSourceLine(parameters& parm, ast::BasicIdentifier* t) {
    printSourceLine(parm, t->text);
  }

  void SystemC::includes(parameters& parm, ast::DesignFile& designFile) {
    functionStart("includes");
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.contextClause) {
        for (ast::UseClause useClause : it->module.contextClause->useClauses.list) {
          println(parm, "using " + listToString(parm, useClause.list, "::", [&](std::string s){return s;}) + ";");
        }
      }
    }    
  }

  void SystemC::enumerationType(parameters& parm, ast::BasicIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = basicIdentifierToString(parm, identifier);
      printSourceLine(parm, identifier);
      std::string enumName = name + "_enum";
      std::string basicIdentifierList = listToString(parm, t->enumerations, ", ",
                                                     [&](std::string s){return s;});
      println(parm, "enum " + enumName + " { " + basicIdentifierList + "};");
      println(parm, "class " + name + " : public Enumeration<" + enumName + "> {");
      println(parm, "public:"); 
      println(parm, "};");
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

  void SystemC::numberType(parameters& parm, ast::BasicIdentifier* identifier, ast::NumberType* t) {
    if (t) {
      std::string name = basicIdentifierToString(parm, identifier);
      std::string left = expressionToString(parm, t->range->left);
      std::string right = expressionToString(parm, t->range->right);
      std::string templateType = "decltype(" + left + ")"; 
      printSourceLine(parm, identifier);
      println(parm, "class " + name + " : public Range<" + templateType + "> {");
      println(parm, "public:"); 
      parm.incIndent();
      println(parm, "explicit " + name + "(" + templateType + " left=" + left +
              ", " + templateType + " right=" + right +
              ") : Range<" + templateType + ">(left, right) {};");
      parm.incIndent();
      println(parm, "using Range<" + templateType + ">::operator=;");
      parm.decIndent();
      println(parm, "};");
      parm.decIndent();
    }
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
    }
  }

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      std::string name = basicIdentifierToString(parm, v->identifier);
      std::string type = basicIdentifierToString(parm, v->type);
      std::string init = "";
      if (v->initialization) {
        init = expressionToString(parm, v->initialization->value);
      }
      DeclarationID id;
      switch (v->objectType) {
      case ast::ObjectDeclaration::SIGNAL: id = SIGNAL; break;
      case ast::ObjectDeclaration::VARIABLE: id = VARIABLE; break;
      case ast::ObjectDeclaration::CONSTANT: id = CONSTANT; break;
      default: assert(false);
      }
      DeclarationInfo i;
      i.id = id;
      parm.declaration[name] = i;
      callback(name, type, init, id, v->direction);
    }
  }

  template<typename Func>
  void SystemC::traverseInterfaceList(parameters& parm, ast::InterfaceList* l, Func callback) {
    functionStart("traverseInterfaceList");
    if (l) {
      for (ast::InterfaceElement i : l->interfaceElements.list) {
        if (i.variable) {objectDeclaration(parm, i.variable, callback);}
        if (i.signal) {objectDeclaration(parm, i.signal, callback);}
        if (i.constant) {objectDeclaration(parm, i.constant, callback);}
      }
    }
    functionEnd("traverseInterfaceList");
  }

  std::string SystemC::objectDeclarationToString(parameters& parm, ast::ObjectDeclaration* v,
                                                 bool initialization) {
    std::string s = "";
    if (v) {
      printSourceLine(parm, v->identifier);
      objectDeclaration(parm, v, [&](std::string& name, std::string& type, std::string& init,
                                     DeclarationID id, ast::ObjectDeclaration::Direction direction) {
          if (id == SIGNAL) {
            type = "sc_signal<" + type + ">";
          }
          s = type + " " + name;
          if (initialization && init.size() > 0) {
            s += " = " + init;
          }
        }
        );
    }
    return s;
  }

  void SystemC::object_declarations(parameters& parm, ast::ObjectDeclaration* v) {
    if (v) {
      printSourceLine(parm, v->identifier);
      println(parm, objectDeclarationToString(parm, v, true) + ";");
    }
  }

  std::string SystemC::interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                             bool initialization) {
    return interfaceListToString(parm, l, delimiter, initialization,
                                 [](std::string& type, DeclarationID id, ast::ObjectDeclaration::Direction direction) {
                                   return type;
                                 });
  }

  template <typename Func>
  std::string SystemC::interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                             bool initialization, Func typeConverter) {
    std::string s;
    if (l) {
      std::string x = "";
      std::string d = "";
      traverseInterfaceList(parm, l, [&](std::string& name, std::string& type, std::string& init,
                                         DeclarationID id, ast::ObjectDeclaration::Direction direction) {
                              s += d + typeConverter(type, id, direction) + " " + name;
          d = delimiter;
        }
        );
    }
    return s;
  }

  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f) {
    if (f) {
      printSourceLine(parm, f->name);
      std::string name = basicIdentifierToString(parm, f->name);
      std::string returnType = basicIdentifierToString(parm, f->returnType);
      std::string interface = "(" + interfaceListToString(parm, f->interface, ", ", false) + ")";
      DeclarationInfo i;
      i.id = FUNCTION;
      parm.declaration[name] = i;
      parm.functions[name] = f;
      println(parm, returnType + " " + name + interface + "{");
      parm.incIndent();
      declarations(parm, f->declarations);
      sequentialStatements(parm, f->sequentialStatements);
      parm.decIndent();
      println(parm, "}");
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    functionStart("declarations");
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      object_declarations(parm, i.variable);
      object_declarations(parm, i.signal);
      object_declarations(parm, i.constant);
      function_declarations(parm, i.function);
    }
    functionEnd("declarations");
  }

  void SystemC::sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l) {
    for (ast::SequentialStatement s : l.list) {
      procedureCallStatement(parm, s.procedureCallStatement);
      variableAssignment(parm, s.variableAssignment);
      signalAssignment(parm, s.signalAssignment);
      reportStatement(parm, s.reportStatement);
      ifStatement(parm, s.ifStatement);
      forLoopStatement(parm, s.forLoopStatement);
      waitStatement(parm, s.waitStatement);
      returnStatement(parm, s.returnStatement);
    }
  }

  template<class Key, class Value, typename Func>
  std::string SystemC::listToString(parameters& parm, std::unordered_map<Key, Value>& t, std::string delimiter, Func callback) {
    std::string s;
    std::string d;
    for (auto x : t) {
      s += (d + callback(x.first, x.second));
      d = delimiter;
    }
    return s;
  }

  template<class T, typename Func>
  std::string SystemC::listToString(parameters& parm, std::list<T>& t, std::string delimiter, Func callback) {
    return listToString(parm, &t, delimiter, callback);
  }
  
  template<class T, typename Func>
  std::string SystemC::listToString(parameters& parm, std::list<T>* t, std::string delimiter, Func callback) {
    std::string s;
    std::string d;
    for (auto x : *t) {
      s += (d + callback(x));
      d = delimiter;
    }
    return s;
  }

  template<typename Func>
  std::string SystemC::listToString(parameters& parm, ast::BasicIdentifierList* list, std::string delimiter, Func callback) {
    std::string s = "";
    if (list) {
      std::string d = "";
      for (ast::BasicIdentifier t : list->textList.list) {
        s += (d + callback(basicIdentifierToString(parm, &t)));
        d = delimiter;
      }
    }
    return s;
  }
  
  std::string SystemC::getConstructorDeclaration(parameters& parm, std::string& name) {
    std::string constructorArguments = "";
    std::string memberVariableAssignment = "";
    if (parm.forGenerateHierarchy.size() > 0) {
      constructorArguments = ", " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return "auto " + s;});
      memberVariableAssignment = ", " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return s + "(" + s + ")";});
    }
    return name + "(" + parm.parentName +"* parent" + constructorArguments + ") : p(parent)" + memberVariableAssignment;
  }

  template <class T, typename Func>
  void SystemC::scThreadShell(parameters& parm, std::string& name, T sensitivity, Func body) {  
    println(parm, "class " + name + " : public sc_thread {");
    parm.incIndent();
    println(parm, parm.parentName + "* p;");
    if (parm.forGenerateHierarchy.size() > 0) {
      println(parm, "INTEGER " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return s;}) + ";");
    }
    parm.decIndent();
    println(parm, "public:");
    parm.incIndent();
    println(parm, getConstructorDeclaration(parm, name) +  + " {};");
    println(parm, "void process() {");
    parm.incIndent();
    if (sensitivity) {
      std::string s = listToString(parm, sensitivity, " || ", [&](std::string s){return s + ".EVENT()";}); 
      println(parm, "wait([&](){return " + s + ";});");
    }
    body(parm);
    parm.decIndent();
    println(parm, "}");
    parm.decIndent();
    println(parm, "};");
  }
  
  void SystemC::methodDefinition(parameters& parm, ast::Method* method) {
    functionStart("methodDefinition");
    if (method) {
      std::string methodName;
      if (method->name) {
        methodName = basicIdentifierToString(parm, method->name);
      } else {
        methodName = "noname" + std::to_string(methodId++);
        method->noname = methodName;
      }
      {
        parameters p = descendHierarchy(parm);
        scThreadShell(p, methodName, method->sensitivity, [&](parameters& parm) {
            declarations(parm, method->declarations);
            sequentialStatements(parm, method->sequentialStatements);
          });
      }
    }
    functionEnd("methodDefinition");
  }

  void SystemC::blockStatementDefinition(parameters& parm,
                                         ast::BlockStatement* blockStatement) {
    functionStart("blockStatementDefinition");
    if (blockStatement) {
      std::string name = basicIdentifierToString(parm, blockStatement->name);
      printSourceLine(parm, blockStatement->name);
      println(parm, "SC_BLOCK(" + name + ") {");
      parm.incIndent();
      println(parm, parm.parentName + "* p;");
      declarations(parm, blockStatement->declarations);
      println(parm, "public:");
      {
        parameters p = parm;
        p.parentName = name;
        concurrentStatementsDefinition(p, blockStatement->concurrentStatements);
      }
      println(parm, getConstructorDeclaration(parm, name) + "{");
      concurrentStatementsInstantiation(parm, blockStatement->concurrentStatements);
      println(parm, "}");
      parm.decIndent();
      println(parm, "};");
    }
  }
  
  void SystemC::forGenerateStatementDefinition(parameters& parm,
                                               ast::ForGenerateStatement* forGenerateStatement) {
    if (forGenerateStatement) {
      parm.forGenerateHierarchy.push_back(basicIdentifierToString(parm, forGenerateStatement->identifier));
      concurrentStatementsDefinition(parm, forGenerateStatement->concurrentStatements);
    }
  }

  void SystemC::concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s) {
    if (s) {
      std::string name = "line" + std::to_string(s->identifier->text.getLine());
      s->name = name;
      std::list<std::string> sensitivity;
      {
        parameters p = descendHierarchy(parm);
        expressionToString(p, s->expression, [&](std::string baseName, std::string hierarchyName) {
            sensitivity.push_back(hierarchyName);
          });
        scThreadShell(p, name, &sensitivity, [&](parameters& parm) {
            signalAssignment(parm, s);
          });
      }
    }
  }
  
  void SystemC::concurrentStatementsDefinition(parameters& parm,
                                               ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    functionStart("concurrentStatementsDefinition");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodDefinition(parm, c.method);
      forGenerateStatementDefinition(parm, c.forGenerateStatement);
      blockStatementDefinition(parm, c.blockStatement);
      concurrentSignalAssignment(parm, c.signalAssignment);
    }
    functionEnd("concurrentStatementsDefinition");
  }

  void SystemC::instantiateType(parameters& parm, std::string type, std::string name) {
    functionStart("instantiateType");
    std::string constructorArguments = "";
    if (parm.forGenerateHierarchy.size() > 0) {
      constructorArguments = ", " + listToString(parm, parm.forGenerateHierarchy, ",", [&](std::string s){return s;});
    }
    println(parm, type + "(new " + name + "(this" + constructorArguments + "));");
  }
  
  void SystemC::methodInstantiation(parameters& parm, ast::Method* method) {
    functionStart("methodInstantiation");
    if (method) {
      std::string methodName;
      if (method->name) {
        methodName = basicIdentifierToString(parm, method->name);
      } else {
        methodName = method->noname;
      }
      instantiateType(parm, "SC_THREAD", methodName);
    }
    functionEnd("methodInstantiation");
  }

  void SystemC::signalInstantiation(parameters& parm, ast::SignalAssignment* s) {
    functionStart("signalInstantiation");
    if (s) {
      instantiateType(parm, "SC_THREAD", s->name);
    }
    functionEnd("signalInstantiation");
  }

  void SystemC::blockStatementInstantiation(parameters& parm,
                                            ast::BlockStatement* blockStatement) {
    functionStart("blockStatementInstantiation");
    if (blockStatement) {
      instantiateType(parm, "SC_NEW_BLOCK", basicIdentifierToString(parm, blockStatement->name));
    }
  }

  void SystemC::forGenerateStatementInstantiation(parameters& parm,
                                                  ast::ForGenerateStatement* forGenerateStatement) {
    
    if (forGenerateStatement) {
      println(parm, rangeTypeToString(parm, forGenerateStatement->identifier, forGenerateStatement->range) + ") {");
      parm.incIndent();
      concurrentStatementsInstantiation(parm, forGenerateStatement->concurrentStatements);
      parm.decIndent();
      println(parm, "}");
    }
  }

  void SystemC::componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l) {
    if (l) {
      println(parm, listToString(parm, l->associationElements.list, "; ",
                                 [&](ast::AssociationElement a){
                                   return instanceName + "." + getName(parm, a.formalPart->name) +
                                     ".bind(" + expressionToString(parm, a.actualPart) + ")";
                                 }) + ";"
              );
    }
  }
  
  void SystemC::componentInstantiation(parameters& parm, ast::ComponentInstance* c) {
    if (c) {
      printSourceLine(parm, c->instanceName);
      std::string instanceName = getName(parm, c->instanceName);
      std::string componentName = "";
      std::string delimiter = "";
      for (ast::BasicIdentifier i : c->componentName->textList.list) {
        std::string n = basicIdentifierToString(parm, &i);
        if (n != "WORK") {
          componentName += delimiter + n;
          delimiter = "::";
        }
      }
      println(parm , "auto " + instanceName + " = " + componentName + "(\"" + instanceName + "\");");
      componentAssociation(parm, instanceName, c->generics);
      componentAssociation(parm, instanceName, c->ports);
    }
  }
  
  void SystemC::concurrentStatementsInstantiation(parameters& parm,
                                                  ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    functionStart("concurrentStatementsInstantiation");
    for (ast::ConcurrentStatement& c : concurrentStatements.list) {
      methodInstantiation(parm, c.method);
      forGenerateStatementInstantiation(parm, c.forGenerateStatement);
      blockStatementInstantiation(parm, c.blockStatement);
      signalInstantiation(parm, c.signalAssignment);
      componentInstantiation(parm, c.componentInstance);
    }
  }

  void SystemC::threadConstructor(parameters& parm, ast::BasicIdentifier* name, 
                                  ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    println(parm, "SC_CTOR(" + basicIdentifierToString(parm, name) + ") {");
    concurrentStatementsInstantiation(parm, concurrentStatements);
    println(parm, "}");
  }
  
  void SystemC::interface(parameters& parm, ast::Interface* intf) {
    if (intf->generics) {
      println(parm, interfaceListToString(parm, intf->generics, "; ", false) + ";");
    }
    if (intf->ports) {
      println(parm, interfaceListToString(parm, intf->ports, "; ", false,
                                          [&](std::string& type, DeclarationID id,
                                              ast::ObjectDeclaration::Direction direction) {
                                            switch (direction) {
                                            case ast::ObjectDeclaration::IN: return "sc_in<" + type + ">";
                                            case ast::ObjectDeclaration::OUT: 
                                            case ast::ObjectDeclaration::INOUT: 
                                            case ast::ObjectDeclaration::BUFFER: return "sc_out<" + type + ">";
                                            }
                                            return type;
                                          }) + ";");
    }
  }

  void SystemC::implementation(parameters& parm, ast::DesignFile& designFile, ast::Interface* interface) {
    functionStart("implementation");
    ast::BasicIdentifier* name = interface->name;
    for (std::list<ast::DesignUnit>::iterator it = designFile.designUnits.list.begin();
         it != designFile.designUnits.list.end(); it++) {
      if (it->module.implementation) {
        if (name->equals(it->module.implementation->name)) {
          declarations(parm, it->module.implementation->declarations);
          concurrentStatementsDefinition(parm, it->module.implementation->concurrentStatements);
          println(parm, "public:");
          parm.incIndent();
          threadConstructor(parm, name, it->module.implementation->concurrentStatements);
          parm.decIndent();
        }
      }
    }
    functionEnd("implementation");
  }

  std::string SystemC::associateArgument(parameters& parm, std::string& name, std::string& init, int argumentNumber, ast::AssociationList* l) { 
    functionStart("associateArgument");
    std::string argument = init;
    if (l) {
      int associationElementNumber = 0;
      for (ast::AssociationElement e : l->associationElements.list) {
        std::string actualPart = expressionToString(parm, e.actualPart);
        if (e.formalPart) {
          std::string formalPart = getName(parm, e.formalPart->name);
          if (formalPart == name) {
            return actualPart;
          }
        } else if (associationElementNumber == argumentNumber) {
          return actualPart;
        }
        associationElementNumber++;
      }
    }
    functionEnd("associateArgument");
    return argument;
  }

  std::string SystemC::parametersToString(parameters& parm, ast::BasicIdentifier* functionName, ast::AssociationList* l) {
    std::string s = "";
    std::string basisName = getName(parm, functionName);
    auto x = parm.functions.find(basisName);
    if (x != parm.functions.end()) {
      ast::FunctionDeclaration* f = x->second;
      /*
        Association list can either be:
        func(formalPart => actualPart, a => w, b => x, c => y)
        or
        func(actualPart, w, x, y)
      */
      std::string delimiter = "";
      int argumentNumber = 0;
      traverseInterfaceList(parm, f->interface,
                            [&](std::string& name, std::string& type, std::string& init, DeclarationID id,
                                ast::ObjectDeclaration::Direction direction) {
                              std::string argument = associateArgument(parm, name, init, argumentNumber, l);
                              if (argument.size() == 0) {
                                printError(functionName->text, "No argument associated element " + std::to_string(argumentNumber));
                              }
                              s += delimiter + argument;
                              delimiter = ", ";
                              argumentNumber++;
                            }
                            );
    } else {
      printWarning(functionName->text, "Could not find function " + basisName + " declaration. Cannot associate arguments.");
      if (l) {
        s = listToString(parm, l->associationElements.list, ",",
                         [&](ast::AssociationElement a){return expressionToString(parm, a.actualPart);});
      }
    }
    return s;
  }
  
  std::string SystemC::procedureCallStatementToString(parameters& parm, ast::ProcedureCallStatement* p) {
    functionStart("procedureCallStatementToString");
    std::string s = "";
    if (p) {
      std::string functionName = getName(parm, p->name, true);
      s = functionName + "(" + parametersToString(parm, p->name, p->associationList) + ")";
    }
    functionEnd("procedureCallStatementToString");
    return s;
  }

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      println(parm, procedureCallStatementToString(parm, p) + ";");
    }
  }
  
  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      println(parm, "report(" + expressionToString(parm, p->message) + ", " +
              basicIdentifierToString(parm, p->severity) + ");");
    }
  }

  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
	  println(parm, command + " (" + expressionToString(parm, c.condition) + ") {");
	} else {
	  println(parm, "} else {");
	}
	command = "} elsif";
        parm.incIndent();
        sequentialStatements(parm, c.sequentialStatements);
        parm.decIndent();
      }
      println(parm, "}");
    }
  }

  std::string SystemC::rangeTypeToString(parameters& parm, ast::BasicIdentifier* i, ast::RangeType* r) {
    std::string s = "for (auto " + basicIdentifierToString(parm, i) + " : " +
      " INTEGER(" + expressionToString(parm, r->left) + ", " +
      expressionToString(parm, r->right) + ")";
    return s;
  }
  
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* p) {
    if (p) {
      println(parm, rangeTypeToString(parm, p->identifier, p->range) + ") {");
      sequentialStatements(parm, p->sequentialStatements);
      println(parm, "}");
    }
  }

  std::string SystemC::physicalToString(parameters& parm, ast::Physical* p) {
    assert (p != NULL);
    return "physical(" + numberToString(parm, p->number) + ", " +
      basicIdentifierToString(parm, p->unit) + ")";
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      println(parm, "wait(" + numberToString(parm, p->physical->number) + ");");
    }
  }

  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      println(parm, basicIdentifierToString(parm, p->identifier) + " = " +
              expressionToString(parm, p->expression) + ";");
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
      println(parm, "return " + expressionToString(parm, r->value) + ";");
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      println(parm, basicIdentifierToString(parm, p->identifier) + " = " +
              expressionToString(parm, p->expression) + ";");
    }
  }

  bool SystemC::matchDeclarationID(parameters& parm, std::string& name, DeclarationID id) {
    auto t = parm.declaration.find(name); 
    if (t == parm.declaration.end()) {
      return false;
    }
    return t->second.id == id;
  }

  int SystemC::getHierarchyLevel(parameters& parm, std::string& name) {
    auto t = parm.declaration.find(name); 
    if (t != parm.declaration.end()) {
      return t->second.hierarchyLevel;
    }
    return 0;
  }

  std::string SystemC::getName(parameters& parm, ast::BasicIdentifier* i, bool hierarchy) {
    assert (i != NULL);
    std::string name = i->text.toString(true);
    std::string s = "";
    if (hierarchy) {
      for (int i=0; i<getHierarchyLevel(parm, name); i++) {
        s += "p->";
      }
    }
    s += name;
    return s;
  }
  
  std::string SystemC::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* i) {
    assert (i != NULL);
    std::string name = getName(parm, i);
    std::string s = getName(parm, i, true);
    if (matchDeclarationID(parm, name, FUNCTION)) {
      std::string parameters = parametersToString(parm, i);
      s += "(" + parameters + ")";
    }
    if (i->attribute) {
      std::string seperator = matchDeclarationID(parm, name, SIGNAL) ? "." : "::";
      s += seperator + i->attribute->toString(true);
      s += "(" + listToString(parm, i->arguments, ",", [&](std::string a){return a;}) + ")";
    }
    return s;
  }

  std::string SystemC::numberToString(parameters& parm, ast::Number* n) {
    assert(n != NULL);
    return n->value.toString();
  }
  
  std::string SystemC::characterToString(parameters& parm, ast::Character* n) {
    assert(n != NULL);
    return n->value.toString();
  }

  template <typename Func>
  std::string SystemC::expressionTermToString(parameters& parm, ast::ExpressionTerm* e, Func basicIdentifierCallback) {
    if (e) {
      if (e->physical) {
        return physicalToString(parm, e->physical);
      }
      if (e->number) {
        return numberToString(parm, e->number);
      }
      if (e->text) {
        return e->text->text.toString();
      }
      if (e->identifier) {
        basicIdentifierCallback(getName(parm, e->identifier), getName(parm, e->identifier, true));
        return basicIdentifierToString(parm, e->identifier);
      }
      if (e->character) {
        return characterToString(parm, e->character);
      }
      if (e->procedureCall) {
        // TODO: Should also get basicIdentifierCallback as argument
        return procedureCallStatementToString(parm, e->procedureCall);
      }
    }
    return "";
  }

  std::string SystemC::expressionToString(parameters& parm, ast::Expression* e) {
    return expressionToString(parm, e, [](std::string baseName, std::string hierarchyName) {});
  }
  
  template <typename Func>
  std::string SystemC::expressionToString(parameters& parm, ast::Expression* e, Func basicIdentifierCallback) {
    assert(e != NULL);
    if (e->parenthis) {
      return "(" + expressionToString(parm, e->parenthis, basicIdentifierCallback) + ")";
    } else if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(parm, e->expression, basicIdentifierCallback);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      default: {assert (false);}
      }
      return op + expr;
    } else if (e->op) {
      std::string op;
      std::string term = expressionTermToString(parm, e->term, basicIdentifierCallback);
      std::string expr = expressionToString(parm, e->expression, basicIdentifierCallback);
      switch (e->op->op) {
      case ::ast::ExpressionOperator::CONCAT: {op = "concat"; break;}
      case ::ast::ExpressionOperator::ADD: {return term + " + " + expr;}
      case ::ast::ExpressionOperator::SUB: {return term + " - " + expr;}
      case ::ast::ExpressionOperator::EQUAL: {return term + " == " + expr;}
      case ::ast::ExpressionOperator::NOT_EQUAL: {return term + " != " + expr;}
      case ::ast::ExpressionOperator::LARGER_OR_EQUAL: {return term + " >= " + expr;}
      case ::ast::ExpressionOperator::SMALLER_OR_EQUAL: {return term + " <= " + expr;}
      default: {assert (false);}
      }
      return op + "(" + term + ", " + expr + ")";
    } else {
      return expressionTermToString(parm, e->term, basicIdentifierCallback);
    }
  }
  
}

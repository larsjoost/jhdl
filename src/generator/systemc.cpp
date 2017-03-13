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
#include "../ast/signal_assignment_condition.hpp"
#include "../ast/report_statement.hpp"
#include "../ast/component_instance.hpp"
#include "../ast/subtype_indication.hpp"

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
        // TODO: Add known functions from std as a work-around
        addDeclarationType(parm, "FINISH", FUNCTION, -1);
        // /TODO
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
      if (it->second.hierarchyLevel >= 0) {
        it->second.hierarchyLevel++;
      }
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
          println(parm, "using " +
                  listToString(parm, useClause.list, "::",
                               [&](ast::BasicIdentifier& b){return getName(parm, &b, false);}) + ";");
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
    subtype:
      using TYPE_T_type = decltype(1);
    type:
      struct TYPE_T_range { TYPE_T_type left = 1; TYPE_T_type right = 20; };
      template<class RANGE = TYPE_T_range>
      using TYPE_T = Range<TYPE_T_type, RANGE>;
  */
  void SystemC::numberType(parameters& parm, ast::BasicIdentifier* identifier, ast::NumberType* t) {
    if (t) {
      printSourceLine(parm, identifier);
      std::string name = basicIdentifierToString(parm, identifier);
      printRangeType(parm, name, t->range);
    }
  }

  /*
  vhdl:
    type type_t is array (0 to 4) of integer range 0 to 10;
  systemc:
    subtype (subtype_declaration):
      struct TYPE_T_subtype { int left = 0; int right = 10; };
      using TYPE_T_type = INTEGER<TYPE_T_subtype>;
    type (printSubtype):
      struct TYPE_T_range { int left = 0; int right = 4; };
      template<class T = TYPE_T_range>
      using TYPE_T = Array<TYPE_T_type, T>;
  vhdl:
    type b_t is array (3 downto -4) of bit;
  */
  void SystemC::arrayType(parameters& parm, ast::BasicIdentifier* identifier, ast::ArrayType* t) {
    if (t) {
      printSourceLine(parm, identifier);
      std::string name = basicIdentifierToString(parm, identifier);
      std::string subtypeName = subtypeIndication(parm, name, t->type) + "<>";
      printArrayType(parm, name, t->range, subtypeName);
    }
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
      arrayType(parm, t->identifier, t->typeDefinition->arrayType);
    }
  }

  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    if (parm.definesAllowed) {
      assert(r);
      std::string left = expressionToString(parm, r->left);
      std::string right = expressionToString(parm, r->right);
      println(parm, "vhdl_range_type(" + name + ", " + left + ", " + right + ");");
    }
  }

  /*
      struct TYPE_T_range { int left = 0; int right = 4; };
      template<class T = TYPE_T_range>
      using TYPE_T = Array<TYPE_T_type, T>;
  */
  void SystemC::printSubtype(parameters& parm, std::string& name, ast::RangeType* r, std::string typeName) {
    if (parm.definesAllowed) {
      assert(r);
      std::string left = expressionToString(parm, r->left);
      std::string right = expressionToString(parm, r->right);
      println(parm, "vhdl_range_subtype(" + name + ", " + typeName + ", " + left + ", " + right + ");");
    }
  }
  
  void SystemC::printArrayType(parameters& parm, std::string& name, ast::RangeType* r, std::string& subtype) {
    assert(r);
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    println(parm, "vhdl_array_type(" + name + ", " + left + ", " + right + ", " + subtype + ");");
  }
  
  /*
  vhdl:
    <name>        <range>
    integer range 0 to 10;
  systemc:
    struct TYPE_T_range { int left = 0; int right = 4; };
    template<class T = TYPE_T_range>
    using TYPE_T = INTEGER<T>;
  */
  std::string SystemC::subtypeIndication(parameters& parm, std::string& name, ast::SubtypeIndication* t) {
    assert(t);
    std::string typeName = basicIdentifierToString(parm, t->name);
    if (t->range) {
      printSubtype(parm, name, t->range, typeName);
      return name;
    }
    return typeName;
  }
    
    /*
  vhdl:
    subtype type_t is integer range 0 to 10;
  systemc:
    struct TYPE_T_range { int left = 0; int right = 4; };
    template<class T = TYPE_T_range>
    using TYPE_T = INTEGER<T>;
  */
  void SystemC::subtype_declarations(parameters& parm, ast::SubtypeDeclaration* t) {
    if (t) {
      printSourceLine(parm, t->identifier);
      std::string name = basicIdentifierToString(parm, t->identifier);
      subtypeIndication(parm, name, t->type);
    }
  }

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      std::string name = basicIdentifierToString(parm, v->identifier);
      std::string type = name + "_type";
      type = subtypeIndication(parm, type, v->type) + "<>";
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
      objectDeclaration(parm, v,
                        [&](std::string& name,
                            std::string& type, std::string& init,
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
      std::string s = objectDeclarationToString(parm, v, true);
      if (parm.instanceAllowed) {
        println(parm, s + ";");
      }
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
      traverseInterfaceList(parm, l, [&](std::string& name,
                                         std::string& type, std::string& init,
                                         DeclarationID id, ast::ObjectDeclaration::Direction direction) {
                              s += d + typeConverter(type, id, direction) + " " + name;
          d = delimiter;
        }
        );
    }
    return s;
  }

  void SystemC::addDeclarationType(parameters& parm, std::string name, DeclarationID id,
                                   int hierarchyLevel) {
      DeclarationInfo i;
      i.id = id;
      i.hierarchyLevel = hierarchyLevel;
      parm.declaration[name] = i;
  }
  
  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f) {
    if (f) {
      std::string name = basicIdentifierToString(parm, f->name);
      addDeclarationType(parm, name, FUNCTION);
      parm.functions[name] = f;
      {
        parameters p = parm;
        printSourceLine(p, f->name);
        std::string returnType = basicIdentifierToString(p, f->returnType) + "<>";
        std::string interface = "(" +
          interfaceListToString(p, f->interface, ", ", false,
                                [](std::string& type, DeclarationID id,
                                   ast::ObjectDeclaration::Direction direction) {
                                  return type;
                                }) + ")";
        println(p, returnType + " " + name + interface + "{");
        p.incIndent();
        declarations(p, f->declarations);
        sequentialStatements(p, f->sequentialStatements);
        p.decIndent();
        println(p, "}");
      }
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    functionStart("declarations");
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      subtype_declarations(parm, i.subtype);
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
  std::string SystemC::listToString(parameters& parm, std::unordered_map<Key, Value>& t,
                                    std::string delimiter, Func callback) {
    std::string s;
    std::string d;
    for (auto x : t) {
      s += (d + callback(x.first, x.second));
      d = delimiter;
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
      s += (d + callback(x));
      d = delimiter;
    }
    return s;
  }

  template<typename Func>
  std::string SystemC::listToString(parameters& parm, ast::BasicIdentifierList* list,
                                    std::string delimiter, Func callback) {
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
  
  template<typename Func, class T>
  std::string SystemC::listToString(parameters& parm, ast::List<T>* list,
                                    std::string delimiter, Func callback) {
    std::string s = "";
    if (list) {
      std::string d = "";
      for (auto t : list->list) {
        s += (d + callback(t));
        d = delimiter;
      }
    }
    return s;
  }

  std::string SystemC::getConstructorDeclaration(parameters& parm, std::string& name) {
    std::string constructorArguments = "";
    std::string memberVariableAssignment = "";
    if (parm.forGenerateHierarchy.size() > 0) {
      constructorArguments = ", " +
        listToString(parm, parm.forGenerateHierarchy, ",",
                     [&](std::string s){return "auto " + s;});
      memberVariableAssignment = ", " +
        listToString(parm, parm.forGenerateHierarchy, ",",
                     [&](std::string s){return s + "(" + s + ")";});
    }
    return name + "(" + parm.parentName +"* parent" + constructorArguments +
      ") : p(parent)" + memberVariableAssignment;
  }

  template <class T, typename DeclarationFunc, typename BodyFunc>
  void SystemC::scThreadShell(parameters& parm, std::string& name, T sensitivity,
                              DeclarationFunc declarations, BodyFunc body) {  
    println(parm, "class " + name + " : public sc_thread {");
    parm.incIndent();
    println(parm, parm.parentName + "* p;");
    if (parm.forGenerateHierarchy.size() > 0) {
      println(parm, "INTEGER<> " + listToString(parm, parm.forGenerateHierarchy,
                                                ",", [&](std::string s){return s;}) + ";");
    }
    parm.decIndent();
    println(parm, "public:");
    parm.incIndent();
    println(parm, getConstructorDeclaration(parm, name) +  + " {};");
    parm.instanceAllowed = false;
    declarations(parm);
    parm.instanceAllowed = true;
    println(parm, "void process() {");
    parm.incIndent();
    parm.definesAllowed = false;
    body(parm);
    parm.definesAllowed = true;
    if (sensitivity) {
      std::string s = listToString(parm, sensitivity, " || ", [&](std::string s){return s + ".EVENT()";}); 
      if (s.size() == 0) {
        println(parm, "wait();");
      } else {
        println(parm, "wait([&](){return " + s + ";});");
      }
    }
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
        scThreadShell(p, methodName, method->sensitivity,
                      [&](parameters& parm) {
                        declarations(parm, method->declarations);
                      },
                      [&](parameters& parm) {
                        declarations(parm, method->declarations);
                        sequentialStatements(parm, method->sequentialStatements);
                      }
                      );
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
      std::string name;
      if (s->label) {
        name = getName(parm, s->label);
      } else {
        name = "line" + std::to_string(s->identifier->text.getLine());
      }
      s->name = name;
      std::list<std::string> sensitivity;
      {
        parameters p = descendHierarchy(parm);
        auto callback = [&](std::string baseName, std::string hierarchyName) {
          sensitivity.push_back(hierarchyName);
        };
        for (ast::SignalAssignmentCondition c : s->signalAssignmentConditions.list) {
          if (c.condition) {
            expressionToString(p, c.condition, callback);
          }
          expressionToString(p, c.expression, callback);
        }
        scThreadShell(p, name, &sensitivity,
                      [&](parameters& parm) {
                        
                      },
                      [&](parameters& parm) {
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
      std::string name = basicIdentifierToString(parm, forGenerateStatement->identifier);
      forLoop(parm, name, forGenerateStatement->range, [&](parameters& parm) {
          concurrentStatementsInstantiation(parm, forGenerateStatement->concurrentStatements);
        });
    }
  }

  void SystemC::componentAssociation(parameters& parm, std::string& instanceName, ast::AssociationList* l) {
    if (l) {
      println(parm, listToString(parm, l->associationElements.list, "; ",
                                 [&](ast::AssociationElement a){
                                   return instanceName + "->" + getName(parm, a.formalPart->name) +
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
      println(parm , "auto " + instanceName + " = new " + componentName + "(\"" + instanceName + "\");");
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
    parm.incIndent();
    concurrentStatementsInstantiation(parm, concurrentStatements);
    parm.decIndent();
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

  std::string SystemC::associateArgument(parameters& parm, std::string& name, std::string& init,
                                         int argumentNumber, ast::AssociationList* l) { 
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
                            [&](std::string& name,
                                std::string& type, std::string& init, DeclarationID id,
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
    if (p) {
      return basicIdentifierToString(parm, p->name);
    }
    functionEnd("procedureCallStatementToString");
    return "";
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

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback) {
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    std::string rangeName = name + "_range";
    println(parm, "struct " + rangeName + "{ int left = " + left + "; int right = " + right + "; };");
    println(parm, "for (auto " + name + " : " + "INTEGER<" + rangeName + ">()) {");
    parm.incIndent();
    callback(parm);
    parm.decIndent();
    println(parm, "}");
  }
  
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* p) {
    if (p) {
      std::string name = basicIdentifierToString(parm, p->identifier);
      forLoop(parm, name, p->range, [&](parameters& parm) {
          sequentialStatements(parm, p->sequentialStatements);
        });
    }
  }

  std::string SystemC::physicalToString(parameters& parm, ast::Physical* p) {
    assert (p != NULL);
    return "TIME<>(" + numberToString(parm, p->number) + ", " +
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
      std::string name = basicIdentifierToString(parm, p->identifier);
      std::string command = "if";
      std::string noConditionCommand = "";
      std::string noConditionDelimiter = "";
      for (ast::SignalAssignmentCondition s : p->signalAssignmentConditions.list) {
        if (s.condition) {
          println(parm, command + " (" + expressionToString(parm, s.condition) + ") {");
          command = "else if";
          noConditionCommand = "else {";
          noConditionDelimiter = "}";
        } else {
          println(parm, noConditionCommand);
        }
        parm.incIndent();
        println(parm, name + " = " + expressionToString(parm, s.expression) + ";");
        parm.decIndent();
        if (s.condition) {
          println(parm, "}");
        } else {
          println(parm, noConditionDelimiter);
        }
      }
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
      std::string parameters = parametersToString(parm, i, i->arguments);
      s += "(" + parameters + ")";
    } else {
      if (i->attribute) {
        bool objectMatch = matchDeclarationID(parm, name, VARIABLE) ||
          matchDeclarationID(parm, name, SIGNAL);
        std::string seperator = objectMatch ? "." : "<>::";
        s += seperator + i->attribute->toString(true);
        if (i->arguments) {
          s += "(" + listToString(parm, i->arguments->associationElements.list, ",",
                                  [&](ast::AssociationElement& a){return expressionToString(parm, a.actualPart);}) + ")";
        }
      } else if (i->arguments) {
        s += listToString(parm, i->arguments->associationElements.list, ",",
                          [&](ast::AssociationElement& a){return "[" + expressionToString(parm, a.actualPart) + "]";});
      }
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
      case ::ast::UnaryOperator::MINUS: {op = "-"; break;}
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

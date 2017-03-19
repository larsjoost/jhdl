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
    if (!quiet) {
      std::cout << std::string(parm.indent, ' ') << text << std::endl;
    }
  }

  void SystemC::printError(ast::Text& t, std::string message) {
    t.printException("error", message);
  }
  
  void SystemC::printWarning(ast::Text& t, std::string message) {
    t.printException("warning", message, std::cout, "// ");
  }

  void SystemC::functionStart(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION START] " << name << std::endl;
    }
  }

  void SystemC::functionEnd(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION END] " << name << std::endl;
    }
  }

  SystemC::SystemC(bool verbose) : verbose(verbose) {}

  void SystemC::generate(ast::DesignFile& designFile) {
    functionStart("SystemC");
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    parameters parm;
    // TODO: Add known functions from std as a work-around
    {
      std::unordered_map<std::string, PackageInfo> m;
      std::string packageName = "ENV";
      addPackageInfo(m, "FINISH", packageName, FUNCTION);
      packageInfo[packageName] = m;
      addPackageInfo(visiblePackageInfo, "BOOLEAN", "STANDARD", TYPE);
      addPackageInfo(visiblePackageInfo, "TRUE", "STANDARD", ENUM);
      addPackageInfo(visiblePackageInfo, "FALSE", "STANDARD", ENUM);
      addPackageInfo(visiblePackageInfo, "INTEGER", "STANDARD", TYPE);
      addPackageInfo(visiblePackageInfo, "NOTE", "STANDARD", ENUM);
      addPackageInfo(visiblePackageInfo, "FAILURE", "STANDARD", ENUM);
    }
    // /TODO
    println(parm, "#include \"systemc.h\"");
    println(parm, "#include \"vhdl.h\"");
    println(parm, "namespace vhdl {");
    parm.incIndent();
    println(parm, "using namespace STANDARD;");
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause);
      packageDeclaration(parm, it.module.package);
      interfaceDeclaration(parm, it.module.interface);
      implementationDeclaration(parm, it.module.implementation);
    }
    parm.decIndent();
    println(parm, "}");
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

  void SystemC::printSourceLine(parameters& parm, ast::SimpleIdentifier* t) {
    printSourceLine(parm, t->text);
  }

  void SystemC::includes(parameters& parm, ast::ContextClause* contextClause) {
    if (contextClause) {
      functionStart("includes");
      for (ast::UseClause& useClause : contextClause->useClauses.list) {
        assert (useClause.package);
        assert (useClause.library);
        std::string library = useClause.library->toString(true);
        if ("WORK" != library) {
          println(parm, "using namespace " + library + ";");
        }
        std::string package = useClause.package->toString(true);
        auto i = packageInfo.find(package);
        if (i != packageInfo.end()) {
          std::string identifier = useClause.identifier->toString(true);
          if ("ALL" == identifier) {
            for (auto j : i->second) {
              visiblePackageInfo[j.first] = j.second; 
            }
          } else {
            auto j = i->second.find(identifier);
            if (j != i->second.end()) {
              visiblePackageInfo[j->first] = j->second;
            } else {
              printError(useClause.identifier->text, "Did not find " + identifier + " is package " + package);
            }
          }
        } else {
          printError(useClause.package->text, "Did not find package " + package);
        }
      }
      functionEnd("includes");
    }    
  }

  void SystemC::enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = identifier->toString(true);
      std::string enumList = listToString(parm, t->enumerations, ", ",
                                          [&](ast::SimpleIdentifier& s){
                                            addDeclarationType(parm, &s, ENUM);
                                            return s.toString();
                                          });
      std::string stringList = listToString(parm, t->enumerations, ", ",
                                          [&](ast::SimpleIdentifier& s){return "(char *)\"" + s.toString() + "\"";});
      /*
        vhdl_enum_type(name, enumArray, stringArray)
        
        enum BOOLEAN_enum {FALSE, TRUE};
        char* BOOLEAN_string[] = {(char *)"false", (char *)"true"};
        template<typename T = BOOLEAN_enum, char* p[] = BOOLEAN_string>
        using BOOLEAN = Enumeration<T, p>;
      */
      //      println(parm, "vhdl_enum_type(" + name + ", vhdl_array({" + enumList + "}), vhdl_array({" + stringList + "}));");
      println(parm, "enum " + name + "_enum {" + enumList + "};");
      println(parm, "char* " + name + "_string[] = {" + stringList + "};");
      println(parm, "template<typename T = " + name + "_enum, char* p[] = " + name + "_string>");
      println(parm, "using " + name + " = Enumeration<T, p>;");
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
  void SystemC::numberType(parameters& parm, ast::SimpleIdentifier* identifier, ast::NumberType* t) {
    if (t) {
      std::string name = identifier->toString(true);
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
  void SystemC::arrayType(parameters& parm, ast::SimpleIdentifier* identifier, ast::ArrayType* t) {
    if (t) {
      std::string name = identifier->toString(true);
      std::string subtypeName = subtypeIndication(parm, name, t->type) + "<>";
      printArrayType(parm, name, t->range, subtypeName);
    }
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      addDeclarationType(parm, t->identifier, TYPE);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
      arrayType(parm, t->identifier, t->typeDefinition->arrayType);
    }
  }

  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    if (scope == DEFINITION || scope == INSTANTIATION) {
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
    if (scope == DEFINITION || scope == INSTANTIATION) {
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
      std::string name = basicIdentifierToString(parm, t->identifier);
      subtypeIndication(parm, name, t->type);
    }
  }

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      assert(v->identifier);
      std::string name = v->identifier->toString(true);
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
      addDeclarationType(parm, v->identifier, id);
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
      printSourceLine(parm, v->identifier->text);
      std::string s = objectDeclarationToString(parm, v, true);
      if (scope == INSTANTIATION) {
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

  void SystemC::addDeclarationType(parameters& parm, ast::SimpleIdentifier* identifier, DeclarationID id) {
      std::string name = identifier->toString(true);
      DeclarationInfo i;
      i.id = id;
      i.hierarchyLevel = 0;
      parm.declaration[name] = i;
  }
  
  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f) {
    if (f) {
      addDeclarationType(parm, f->name, FUNCTION);
      std::string name = f->name->toString(true);
      parm.functions[name] = f;
      {
        parameters p = parm;
        printSourceLine(p, f->name->text);
        std::string returnType = f->returnType->toString(true) + "<>";
        std::string interface = "(" +
          interfaceListToString(p, f->interface, ", ", false,
                                [](std::string& type, DeclarationID id,
                                   ast::ObjectDeclaration::Direction direction) {
                                  return type;
                                }) + ")";
        if (f->body) {
          std::string s = (scope == IMPLEMENTATION) ? p.parentName + "::" : "";
          println(p, returnType + " " + s + name + interface + "{");
          p.incIndent();
          function_body(p, f->body);
          p.decIndent();
          println(p, "}");
        } else {
          println(p, returnType + " " + name + interface + ";");
        }
      }
    }
  }

  void SystemC::function_body(parameters& parm, ast::FunctionBody* f) {
    assert(f);
    declarations(parm, f->declarations);
    sequentialStatements(parm, f->sequentialStatements);
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    functionStart("declarations");
    println(parm, "// Declarations");
    for (ast::Declaration i : d.list) {
      if (scope == DEFINITION) {
        type_declarations(parm, i.type);
        subtype_declarations(parm, i.subtype);
      } else {
        object_declarations(parm, i.variable);
        object_declarations(parm, i.signal);
        object_declarations(parm, i.constant);
        function_declarations(parm, i.function);
      }
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
    functionStart("scThreadShell");
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
    scope = DEFINITION;
    declarations(parm);
    println(parm, "void process() {");
    parm.incIndent();
    scope = INSTANTIATION;
    body(parm);
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
    functionEnd("scThreadShell");
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
      parm.forGenerateHierarchy.push_back(forGenerateStatement->identifier->toString(true));
      concurrentStatementsDefinition(parm, forGenerateStatement->concurrentStatements);
    }
  }

  void SystemC::concurrentSignalAssignment(parameters& parm, ast::SignalAssignment* s) {
    
    if (s) {
      functionStart("concurrentSignalAssignment");
      std::string name;
      if (s->label) {
        name = getName(parm, s->label);
      } else {
        name = "line" + std::to_string(s->identifier->text.getLine());
      }
      s->name = name;
      
      {
        parameters p = descendHierarchy(parm);
        std::list<std::string> sensitivity;
        quiet = true;
        signalAssignment(p, s, [&](std::string baseName, std::string hierarchyName) {
            sensitivity.push_back(hierarchyName);
          });
        quiet = false;
        scThreadShell(p, name, &sensitivity,
                      [&](parameters& parm) { },
                      [&](parameters& parm) {
                        signalAssignment(parm, s);
                      });
      }
      functionEnd("concurrentSignalAssignment");
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
      assert(forGenerateStatement->identifier);
      std::string name = forGenerateStatement->identifier->toString(true);
      parameters p = parm;
      addDeclarationType(p, forGenerateStatement->identifier, VARIABLE);
      forLoop(p, name, forGenerateStatement->range, [&](parameters& parm) {
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
      assert(c->instanceName);
      printSourceLine(parm, c->instanceName->text);
      std::string instanceName = c->instanceName->toString(true);
      std::string componentName = "";
      std::string delimiter = "";
      for (ast::SimpleIdentifier i : c->componentName->list) {
        std::string n = i.toString(true);
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

  void SystemC::threadConstructor(parameters& parm, ast::SimpleIdentifier* name, 
                                  ast::List<ast::ConcurrentStatement>& concurrentStatements) {
    assert(name);
    println(parm, "SC_CTOR(" + name->toString(true) + ") {");
    parm.incIndent();
    concurrentStatementsInstantiation(parm, concurrentStatements);
    parm.decIndent();
    println(parm, "}");
  }

  void SystemC::addPackageInfo(std::unordered_map<std::string, PackageInfo>& m,
                               std::string name, std::string packageName,
                               DeclarationID id) {
    PackageInfo p;
    p.id = id;
    p.name = packageName;
    m[name] = (p);
  }
  
  void SystemC::savePackageInfo(parameters& parm, std::string& packageName) {
    std::unordered_map<std::string, PackageInfo> m;
    // std::cout << "PACKAGE INFO of " << packageName << std::endl;
    for (auto i : parm.declaration) {
      addPackageInfo(m, i.first, packageName, i.second.id);  
    }
    packageInfo[packageName] = m;
  }
  
  void SystemC::packageDeclaration(parameters& parm, ast::Package* package) {
    if (package) {
      functionStart("package");
      std::string name = package->name->toString(true);
      parm.parentName = name;
      scope = DEFINITION;
      parameters p = parm;
      declarations(p, package->declarations);
      if (!package->body) {
        scope = INSTANTIATION;
        println(parm, "");
        println(parm, "SC_PACKAGE(" + name + ") {");
        parm.incIndent();
      } else {
        scope = IMPLEMENTATION;
        println(parm, "// Package body of " + name);
      }
      declarations(p, package->declarations);
      if (!package->body) {
        savePackageInfo(p, name);
        parm.decIndent();
        println(parm, "} " + name + ";");
      }
      functionEnd("package");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface) {
    if (interface) {
      functionStart("interface");
      std::string name = interface->name->toString(true);
      parm.parentName = name;
      println(parm, "");
      println(parm, "SC_INTERFACE(" + name + ") {");
      println(parm, "public:");
      parm.incIndent();
      if (interface->generics) {
        println(parm, interfaceListToString(parm, interface->generics, "; ", false) + ";");
      }
      if (interface->ports) {
        println(parm, interfaceListToString(parm, interface->ports, "; ", false,
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
      parm.decIndent();
      println(parm, "};");
      functionEnd("interface");
    }
  }

  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation) {
    if (implementation) {
      functionStart("implementation");
      std::string name = implementation->name->toString(true);
      parm.parentName = name;
      scope = DEFINITION;
      declarations(parm, implementation->declarations);
      println(parm, "");
      println(parm, "SC_MODULE(" + name + ") {");
      println(parm, "public:");
      parm.incIndent();
      scope = INSTANTIATION;
      declarations(parm, implementation->declarations);
      scope = IMPLEMENTATION;
      concurrentStatementsDefinition(parm, implementation->concurrentStatements);
      println(parm, "public:");
      parm.incIndent();
      threadConstructor(parm, implementation->name, implementation->concurrentStatements);
      parm.decIndent();
      parm.decIndent();
      println(parm, "};");
      functionEnd("implementation");
    }
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
    std::string s = "";
    if (p) {
      functionStart("procedureCallStatementToString");
      s = basicIdentifierToString(parm, p->name);
      functionEnd("procedureCallStatementToString");
    }
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
  
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f) {
    if (f) {
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      parameters p = parm;
      addDeclarationType(p, f->identifier, VARIABLE);
      forLoop(p, name, f->range, [&](parameters& parm) {
          sequentialStatements(parm, f->sequentialStatements);
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
    signalAssignment(parm, p,  [](std::string baseName, std::string hierarchyName) {});
  }

  template <typename Func>
  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p, Func callback) {
    functionStart("signalAssignment");
    if (p) {
      printSourceLine(parm, p->identifier);
      std::string name = basicIdentifierToString(parm, p->identifier);
      std::string command = "if";
      std::string noConditionCommand = "";
      std::string noConditionDelimiter = "";
      for (ast::SignalAssignmentCondition s : p->signalAssignmentConditions.list) {
        if (s.condition) {
          println(parm, command + " (" + expressionToString(parm, s.condition, callback) + ") {");
          command = "else if";
          noConditionCommand = "else {";
          noConditionDelimiter = "}";
        } else {
          println(parm, noConditionCommand);
        }
        parm.incIndent();
        println(parm, name + " = " + expressionToString(parm, s.expression, callback) + ";");
        parm.decIndent();
        if (s.condition) {
          println(parm, "}");
        } else {
          println(parm, noConditionDelimiter);
        }
      }
    }
    functionEnd("signalAssignment");
  }

  bool SystemC::getIdentifierInfo(parameters& parm, ast::BasicIdentifier* identifier, IdentifierInfo& info) {
    functionStart("getIdentifierInfo");
    assert (identifier);
    std::string name = identifier->text.toString(true);
    auto t = parm.declaration.find(name); 
    bool result = true;
    if (t != parm.declaration.end()) {
      info.id = t->second.id;
      info.packageName = "WORK";
      info.hierarchyLevel = t->second.hierarchyLevel;
    } else {
      auto t = visiblePackageInfo.find(name);
      if (t != visiblePackageInfo.end()) {
        info.packageName = t->second.name;
        info.id = t->second.id;
        info.hierarchyLevel = -1;
      } else {
        result = false;
        std::string s = "";
        for (auto i : packageInfo) {
          auto x = i.second.find(name);
          if (x != i.second.end()) {
            s = ". Found " + name + " in package " + i.first + ". Maybe you forgot to declare it in an USE statement.";
          }
        }
        printError(identifier->text, "Could not find declaration of identifier " + name + s);
      }
    }
    functionEnd("getIdentifierInfo");
    return result;
  }
  
  bool SystemC::matchDeclarationID(parameters& parm, ast::BasicIdentifier* identifier, DeclarationID id) {
    functionStart("matchDeclarationID");
    IdentifierInfo info;
    bool result = false;
    if (getIdentifierInfo(parm, identifier, info)) {
      result = (info.id == id);
    }
    functionEnd("matchDeclarationID");
    return result;
  }

  std::string SystemC::getNamePrefix(IdentifierInfo& info) {
    std::string s = "";
    if (info.id == ENUM || info.id == TYPE) {
    } else if (info.hierarchyLevel >= 0) {
      for (int i=0; i<info.hierarchyLevel; i++) {
        s += "p->";
      }
    } else {
      s = info.packageName + ".";
    }
    return s;
  }
  
  std::string SystemC::getName(parameters& parm, ast::BasicIdentifier* identifier, bool hierarchy) {
    functionStart("getName");
    assert (identifier != NULL);
    std::string s = "";
    std::string name = identifier->text.toString(true);
    if (hierarchy) {
      IdentifierInfo info;
      if (getIdentifierInfo(parm, identifier, info)) {
        s = getNamePrefix(info);
      }
    }
    functionEnd("getName");
    return s + name;
  }
  
  std::string SystemC::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* identifier) {
    functionStart("basicIdentifierToString");
    assert (identifier);
    IdentifierInfo info;
    std::string name = identifier->text.toString(true);
    if (getIdentifierInfo(parm, identifier, info)) {
      name = getNamePrefix(info) + name;
      if (info.id == FUNCTION) {
        std::string parameters = parametersToString(parm, identifier, identifier->arguments);
        name += "(" + parameters + ")";
      } else {
        if (identifier->attribute) {
          bool objectMatch = (info.id == VARIABLE) || (info.id == SIGNAL);
          std::string seperator = objectMatch ? "." : "<>::";
          name += seperator + identifier->attribute->toString(true);
          std::string arguments = "";
          if (identifier->arguments) {
            arguments = listToString(parm, identifier->arguments->associationElements.list, ",",
                                     [&](ast::AssociationElement& a){return expressionToString(parm, a.actualPart);});
          }
          name += "(" + arguments + ")";
        } else if (identifier->arguments) {
          name += listToString(parm, identifier->arguments->associationElements.list, ",",
                            [&](ast::AssociationElement& a){return "[" + expressionToString(parm, a.actualPart) + "]";});
        }
      }
    }
    functionEnd("basicIdentifierToString");
    return name;
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

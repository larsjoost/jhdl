#include <iostream>
#include <list>
#include <cassert>


#include "systemc.hpp"
#include "sequential.hpp"
#include "expression.hpp"
#include "declarations.hpp"
#include "definition.hpp"

namespace generator {

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
      addPackageInfo(visiblePackageInfo, "TIME", "STANDARD", TYPE);
      addPackageInfo(visiblePackageInfo, "BIT", "STANDARD", TYPE);
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


  void SystemC::enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = identifier->toString(true);
      std::string enumList = listToString(parm, t->enumerations, ", ",
                                          [&](ast::SimpleIdentifier& s){
                                            addDeclarationType(parm, &s, ENUM);
                                            return s.toString();
                                          });
      std::string stringList = listToString(parm, t->enumerations, ", ",
                                          [&](ast::SimpleIdentifier& s){return "\"" + s.toString() + "\"";});
      /*
        vhdl_enum_type(name, enumArray, stringArray)
        
        enum BOOLEAN_enum {FALSE, TRUE};
        char* BOOLEAN_string[] = {(char *)"false", (char *)"true"};
        template<typename T = BOOLEAN_enum, char* p[] = BOOLEAN_string>
        using BOOLEAN = Enumeration<T, p>;
      */
      //      println(parm, "vhdl_enum_type(" + name + ", vhdl_array({" + enumList + "}), vhdl_array({" + stringList + "}));");
      std::string enumName = name + "_enum";
      std::string stringName = name + "_string";
      println(parm, "enum " + enumName + " {" + enumList + "};");
      println(parm, "struct " + stringName + " {");
      parm.incIndent();
      println(parm, "std::string toString(" + enumName + " e) {");
      parm.incIndent();
      println(parm, "static std::string s[] = {" + stringList + "};");
      println(parm, "return s[(int)e];");
      parm.decIndent();
      println(parm, "}");
      parm.decIndent();
      println(parm, "};");
      println(parm, "template<typename T = " + enumName + ", class p = " + stringName + ">");
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


  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    assert(r);
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    println(parm, "vhdl_range_type(" + name + ", " + left + ", " + right + ");");
  }

  /*
      struct TYPE_T_range { int left = 0; int right = 4; };
      template<class T = TYPE_T_range>
      using TYPE_T = Array<TYPE_T_type, T>;
  */
  void SystemC::printSubtype(parameters& parm, std::string& name, ast::RangeType* r, std::string typeName) {
    assert(r);
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    println(parm, "vhdl_range_subtype(" + name + ", " + typeName + ", " + left + ", " + right + ");");
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
    std::string typeName = t->name->toString(true);
    if (t->range) {
      printSubtype(parm, name, t->range, typeName);
      return name;
    }
    return typeName;
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
      println(parm, s + ";");
    }
  }

  void SystemC::function_body(parameters& parm, ast::FunctionBody* f) {
    assert(f);
    functionStart("function_body");
    declarations(parm, f->declarations);
    sequentialStatements(parm, f->sequentialStatements);
    functionEnd("function_body");
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
      parameters p = parm;
      if (!package->body) {
        println(parm, "");
        println(parm, "SC_PACKAGE(" + name + ") {");
        parm.incIndent();
      } else {
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
      defineObject(parm,
                   name,
                   "SC_MODULE",
                   NULL,
                   &implementation->declarations,
                   &implementation->concurrentStatements,
                   [&](parameters& parm){});
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
    functionStart("parametersToString");
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
      traverseInterfaceList(parm, f->interface, false,
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
    functionEnd("parametersToString");
    return s;
  }
  
  
}

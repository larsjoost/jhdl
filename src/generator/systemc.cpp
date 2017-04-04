#include <iostream>
#include <list>
#include <cassert>
#include <algorithm>

#include "../parser/design_file.hpp"


#include "systemc.hpp"
#include "sequential.hpp"
#include "expression.hpp"
#include "declarations.hpp"
#include "definition.hpp"

namespace generator {

  SystemC::SystemC(bool verbose) : verbose(verbose) {}

  
  void SystemC::generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename) {
    functionStart("SystemC");
    filename = designFile.filename;
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    parameters parm;
    libraryInfo.load(libraryInfoFilename);
    if (!configurationFilename.empty()) {
      if (!config.load(configurationFilename)) {
        std::cerr << "Could not open configuration file " << configurationFilename << std::endl;
      } else {
        parsePackage(parm, "standard", "std");
      }
    }
    println(parm, "#include \"systemc.h\"");
    println(parm, "#include \"vhdl.h\"");
    println(parm, "namespace vhdl {");
    parm.incIndent();
    // transform(library.begin(), library.end(), library.begin(), toupper);
    // println(parm, "namespace " + library + " {");
    // parm.incIndent();
    //    println(parm, "using namespace STANDARD;");
    parse(parm, designFile, library);
    // parm.decIndent();
    // println(parm, "}");
    parm.decIndent();
    println(parm, "}");
  }

  void SystemC::saveLibraryInfo() {
    libraryInfo.save();
  }

  void SystemC::addLibraryInfo(std::string section, std::string name, std::string filename) {
    if (!quiet) {
      libraryInfo.add("package", name, filename);
    }
  }
  
  void SystemC::parse(parameters& parm, ast::DesignFile& designFile, std::string& library) {
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause);
      packageDeclaration(parm, it.module.package);
      interfaceDeclaration(parm, it.module.interface);
      implementationDeclaration(parm, it.module.implementation);
    }
  }

  void SystemC::parsePackage(parameters& parm, std::string name, std::string library) {
    std::string stdPath = config.find("libraries", library);
    if (!stdPath.empty()) {
      Config c;
      c.load(stdPath + "/" + libraryInfoFilename);
      std::string filename = c.find("package", name);
      if (!filename.empty()) {
        filename = stdPath + "/" + filename;
        bool q = quiet;
        quiet = true;
        auto parserDesignFile = parser::DesignFile(verbose);
        parserDesignFile.parse(filename);
        parse(parm, parserDesignFile, library);
        quiet = q;
      }
    }
  }
  
  void SystemC::enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t) {
    if (t) {
      std::string name = identifier->toString(true);
      std::string enumList =
        listToString(parm, t->enumerations, ", ",
                     [&](ast::EnumerationElement& e){
                       std::string s = "";
                       if (e.identifier) {
                         addDeclarationType(parm, e.identifier, ENUM);
                         s = e.identifier->toString();
                       }
                       return s;
                     });
      int size = 0;
      std::string enumName = name + "_enum";
      std::string structList =
        listToString(parm, t->enumerations, ", ",
                     [&](ast::EnumerationElement& e){
                       size++;
                       std::string s;
                       if (e.identifier) {
                         std::string a = e.identifier->toString(true);
                         std::string b = e.identifier->toString();
                         s =  a + ", 0, \"" + b + "\"";
                       } else if (e.character) {
                         s = "(" + enumName + ")0, " + e.character->toString() + ", \"\"";
                       } 
                       s = "{" + s + "}";
                       return s;
                     });
      /*
        TYPE state_t IS (IDLE, '1', STOP);

        vhdl_enum_type(name, enumArray, stringArray)
        
        enum STATE_T_enum {IDLE, STOP};
        EnumerationElement<STATE_T_enum> STATE_T_value[3] = { {IDLE, "idle"}, {'1'}, {STOP, "stop"}};
        using STATE_T = Enumeration<STATE_T_value, 3>;
      */
      //      println(parm, "vhdl_enum_type(" + name + ", vhdl_array({" + enumList + "}), vhdl_array({" + stringList + "}));");
      std::string valueName = name + "_value";
      std::string s = std::to_string(size);
      println(parm, "enum " + enumName + " {" + enumList + "};");
      println(parm, "struct " + valueName + " {");
      parm.incIndent();
      println(parm, "EnumerationElement<" + enumName + "> array[" + s + "] {" + structList + "};");
      parm.decIndent();
      println(parm, "};");
      println(parm, "using " + name + " = Enumeration<" + enumName + ", " + valueName + ", " + s + ">;");
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
      if (t->physical) {
        printPhysicalType(parm, name, t);
      } else {
        printRangeType(parm, name, t->range);
      }
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
      std::string subtypeName = subtypeIndication(parm, name, t->type);
      printArrayType(parm, name, t->definition, subtypeName);
    }
  }


  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    assert(r);
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    println(parm, "using " + name + "_type = decltype(" + left + ");"); 
    println(parm, "vhdl_range_type(" + name + ", " + left + ", " + right + ");");
  }

  void SystemC::printPhysicalType(parameters& parm, std::string& name, ast::NumberType* n) {
    assert(n);
    ast::RangeType* r = n->range;
    assert(r);
    ast::PhysicalType* p = n->physical;
    assert(p);
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    std::string s = listToString(parm, p->elements.list, ", ",
                                 [&](ast::PhysicalElement& e){
                                   return e.unit->toString(true);
                                 });
    std::string enumName = name + "_enum";
    println(parm, "enum " + enumName + " {" + s + "};");
    int size = 0;
    s = listToString(parm, p->elements.list, ", ",
                     [&](ast::PhysicalElement& e){
                       size++;
                       bool baseUnit = (e.physical == NULL);
                       std::string u = e.unit->toString(true);
                       std::string number = baseUnit ? "1" : e.physical->number->toString(); 
                       std::string unit = baseUnit ? u : e.physical->unit->toString(true);
                       std::string base = baseUnit ? "true" : "false";
                       return "{" + u + ", " + number + ", " + unit + "}";
                     });
    std::string valueName = name + "_value";
    println(parm, "struct " + valueName + " {");
    parm.incIndent();
    {
      std::string x = std::to_string(size);
      println(parm, "const int size = " + x + ";");
      println(parm, "const PhysicalElement<" + enumName + ", decltype(" + left + ")> array[" + x + "] {" + s + "};");
    }
    parm.decIndent();
    println(parm, "};");
    std::string typeName = name + "_type";
    println(parm, "using " + typeName + " = Physical<decltype(" + left + "), " + enumName + ">;"); 
    println(parm, "vhdl_physical_type(" + name + ", " + left + ", " + right + ", " + enumName + ", " + valueName + ");");
  }

  /*
   * vhdl_range_subtype examples:
   *
   * subtype NATURAL is INTEGER range 0 to INTEGER'HIGH;
   * subtype DELAY is TIME range 0 fs to TIME'HIGH;

   * struct TYPE_T_range { int left = 0; int right = 4; };
   * template<class T = TYPE_T_range>
   * using TYPE_T = Array<TYPE_T_type, T>;
  */
  
  void SystemC::printSubtype(parameters& parm, std::string& name, ast::RangeType* r, std::string typeName) {
    assert(r);
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
    std::string rangeName = name + "_range";
    println(parm, "struct " + rangeName + " {");
    parm.incIndent();
    println(parm, typeName + "_type left = " + left + ";");
    println(parm, typeName + "_type right = " + right + ";");
    parm.decIndent();
    println(parm, "};");
    println(parm, "template<class RANGE = " + rangeName + ">");
    println(parm, "using " + name + " = " + typeName + "<RANGE>;");
  }
  
  void SystemC::printArrayType(parameters& parm, std::string& name, ast::ArrayDefinition* r, std::string& subtype) {
    assert(r);
    if (r->range) {
      std::string left = expressionToString(parm, r->range->left);
      std::string right = expressionToString(parm, r->range->right);
      println(parm, "vhdl_array_type(" + name + ", " + left + ", " + right + ", " + subtype + ");");
    } else if (r->subtype) {
      std::string id = r->subtype->identifier->toString(true);
      println(parm, "vhdl_array_type(" + name + ", " + id + "<>::LEFT(), " + id + "<>::RIGHT(), " + subtype + ");");
    }
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
        println(p, "");
        println(p, "SC_PACKAGE(" + name + ") {");
        p.incIndent();
      } else {
        println(p, "// Package body of " + name);
      }
      bool implementationArea = (package->body) ? true : false;
      declarations(p, package->declarations, implementationArea);
      if (!package->body) {
        savePackageInfo(p, name);
        addLibraryInfo("package", name, filename);
        p.decIndent();
        println(p, "} " + name + ";");
      } else {
        addLibraryInfo("body", name, filename);
      }
      functionEnd("package");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface) {
    if (interface) {
      functionStart("interface");
      std::string name = interface->name->toString(true);
      addLibraryInfo("entity", name, filename);
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
      addLibraryInfo("architecture", name, filename);
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

#include <iostream>
#include <list>
#include <cassert>
#include <algorithm>

#include "../parser/design_file.hpp"


#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"
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
        exceptions.printError("Could not open configuration file " + configurationFilename);
      } else {
        loadPackage(parm, "STANDARD", "STD", "ALL");
      }
    }
    println(parm, "#include \"systemc.h\"");
    println(parm, "#include \"vhdl.h\"");
    println(parm, "#include \"standard.h\"");
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
      libraryInfo.add(section, name, filename);
    }
  }
  
  void SystemC::parse(parameters& parm, ast::DesignFile& designFile, std::string& library) {
    functionStart("parse");
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause);
      packageDeclaration(parm, it.module.package, library);
      interfaceDeclaration(parm, it.module.interface, library);
      implementationDeclaration(parm, it.module.implementation, library);
    }
    functionEnd("parse");
  }

  void SystemC::parsePackage(parameters& parm, std::string name, std::string library) {
    functionStart("parsePackage(library = " + library + ", name = " + name + ")");
    std::string stdPath = (library == "WORK") ? "." : config.find("hdl", library);
    if (!stdPath.empty()) {
      Config c;
      c.load(stdPath + "/" + libraryInfoFilename);
      std::string filename = c.find("package", name);
      if (!filename.empty()) {
	if (filename != this->filename) {
	  filename = stdPath + "/" + filename;
	  bool q = quiet;
	  quiet = true;
	  parser::DesignFile parserDesignFile;
	  parserDesignFile.parse(filename);
	  parse(parm, parserDesignFile, library);
	  quiet = q;
	} else {
	  exceptions.printError("Could not find package \"" + name + "\" in file " + filename);
	}
      } else {
        exceptions.printError("Could not resolve filename of package \"" + name + "\"");
      }
    } else {
      exceptions.printError("Could not find library \"" + library + "\" is [hdl] section of config file");
    }
    functionEnd("parsePackage");
  }
  
  ast::ObjectValueContainer SystemC::enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t) {
    assert(t); 
    std::string name = identifier->toString(true);
    std::string enumList =
      listToString(parm, t->enumerations, ", ",
                   [&](ast::EnumerationElement& e){
                     std::string s = "";
                     if (e.identifier) {
                       s = e.identifier->toString();
                       database.add(ast::ENUM, s);
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
    return ast::ObjectValueContainer(name);
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
  ast::ObjectValueContainer SystemC::numberType(parameters& parm, ast::SimpleIdentifier* identifier, ast::NumberType* t) {
    assert(t);
    std::string name = identifier->toString(true);
    ast::ObjectValue value;
    if (t->physical) {
      printPhysicalType(parm, name, t);
      value = ast::PHYSICAL;
    } else {
      printRangeType(parm, name, t->range);
      value = ast::INTEGER;
    }
    return ast::ObjectValueContainer(value, name);
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
  ast::ObjectValueContainer SystemC::arrayType(parameters& parm, ast::SimpleIdentifier* identifier, ast::ArrayType* t) {
    assert(t); 
    std::string name = identifier->toString(true);
    ast::ObjectValueContainer value;
    std::string subtypeName = subtypeIndication(parm, value, name, t->type);
    printArrayType(parm, name, t->definition, subtypeName);
    return value;
  }

  void SystemC::rangeToString(ast::RangeType* r, std::string& left, std::string& right, ast::ObjectValueContainer& expectedType) {
    assert(r);
    ast::ObjectValueContainer actualType;
    ExpressionParser expr(&database);
    expr.getType(r->left, expectedType, actualType);
    left = expr.toString(r->left, actualType);
    right = expr.toString(r->right, actualType);
  }

  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    std::string left, right;
    ast::ObjectValueContainer type(ast::NUMBER);
    rangeToString(r, left, right, type);
    println(parm, "using " + name + "_type = decltype(" + left + ");"); 
    println(parm, "vhdl_range_type(" + name + ", " + left + ", " + right + ");");
  }

  void SystemC::printPhysicalType(parameters& parm, std::string& name, ast::NumberType* n) {
    assert(n);
    ast::RangeType* r = n->range;
    assert(r);
    ast::PhysicalType* p = n->physical;
    assert(p);
    std::string left, right;
    ast::ObjectValueContainer type(ast::NUMBER);
    rangeToString(r, left, right, type);
    std::string s = listToString(parm, p->elements.list, ", ",
                                 [&](ast::PhysicalElement& e){
                                   std::string unit = e.unit->toString(true); 
                                   database.add(ast::ENUM, unit);
                                   return unit;
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
  
  void SystemC::printSubtype(parameters& parm, std::string& name, ast::RangeType* r, std::string typeName, ast::ObjectValueContainer& type) {
    std::string left, right;
    rangeToString(r, left, right, type);
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
      std::string left, right;
      ast::ObjectValueContainer type(ast::INTEGER);
      rangeToString(r->range, left, right, type);
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
  std::string SystemC::subtypeIndication(parameters& parm, ast::ObjectValueContainer& value,
                                         std::string& name, ast::SubtypeIndication* t) {
    assert(t);
    std::string typeName = t->name->toString(true);
    auto valid = [&](DatabaseElement* e) {
      return e->id == ast::TYPE;
    };
    DatabaseResult object;
    if (database.findOne(object, typeName, valid)) { 
      value = object.object->type;
      if (t->range) {
        printSubtype(parm, name, t->range, typeName, value);
        return name;
      }
    } else {
      exceptions.printError("Could not fin type \"" + typeName + "\"", &t->name->text);
    }
    return typeName;
  }
    

  std::string SystemC::objectDeclarationToString(parameters& parm, ast::ObjectDeclaration* v,
                                                 bool initialization) {
    std::string s = "";
    if (v) {
      auto func = [&](std::string& name,
		      std::string& type, std::string& init,
		      ast::ObjectType id, ast::ObjectDeclaration::Direction direction) {
	if (id == ast::SIGNAL) {
	  type = "sc_signal<" + type + ">";
	}
	s = type + " " + name;
	if (initialization && init.size() > 0) {
	  s += " = " + init;
	}
      };
      objectDeclaration(parm, v, func);
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

  void SystemC::packageDeclaration(parameters& parm, ast::Package* package, std::string& library) {
    if (package) {
      functionStart("packageDeclaration(library = " + library + ")");
      std::string name = package->name->toString(true);
      addLibraryInfo("package", name, filename);
      database.setLibrary(library);
      database.setPackage(name);
      descendHierarchy(parm, name);
      if (!package->body) {
        println(parm, "");
        println(parm, "SC_PACKAGE(" + name + ") {");
        parm.incIndent();
      } else {
        println(parm, "// Package body of " + name);
      }
      bool implementationArea = (package->body) ? true : false;
      declarations(parm, package->declarations, implementationArea);
      if (!package->body) {
        parm.decIndent();
        println(parm, "} " + name + ";");
      }
      database.globalize();
      ascendHierarchy(parm);
      functionEnd("packageDeclaration");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    if (interface) {
      functionStart("interface");
      std::string name = interface->name->toString(true);
      addLibraryInfo("entity", name, filename);
      database.setLibrary(library);
      database.setEntity(name);
      descendHierarchy(parm, name);
      println(parm, "");
      println(parm, "SC_INTERFACE(" + name + ") {");
      println(parm, "public:");
      parm.incIndent();
      if (interface->generics) {
        println(parm, interfaceListToString(parm, interface->generics, "; ", false) + ";");
      }
      if (interface->ports) {
	auto func = [&](std::string& type, ast::ObjectType id,
			ast::ObjectDeclaration::Direction direction) {
	  switch (direction) {
	  case ast::ObjectDeclaration::IN: return "sc_in<" + type + ">";
	  case ast::ObjectDeclaration::OUT: 
	  case ast::ObjectDeclaration::INOUT: 
	  case ast::ObjectDeclaration::BUFFER: return "sc_out<" + type + ">";
	  }
	  return type;
	};
        println(parm, interfaceListToString(parm, interface->ports, "; ", false, func) + ";");
      }
      parm.decIndent();
      println(parm, "};");
      database.globalize();
      ascendHierarchy(parm);
      functionEnd("interface");
    }
  }

  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library) {
    if (implementation) {
      functionStart("implementation");
      std::string name = implementation->name->toString(true);
      addLibraryInfo("architecture", name, filename);
      database.setLibrary(library);
      database.setArchitecture(name);
      descendHierarchy(parm, name);
      defineObject(parm,
                   name,
                   "SC_MODULE",
                   NULL,
                   &implementation->declarations,
                   &implementation->concurrentStatements,
                   [&](parameters& parm){});
      ascendHierarchy(parm);
      functionEnd("implementation");
    }
  }

  
}

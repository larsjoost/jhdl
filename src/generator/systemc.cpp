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

  SystemC::SystemC(bool verbose) {
    this->verbose |= verbose; 
  }

  void SystemC::generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename) {
    functionStart("SystemC");
    filename = designFile.filename;
    parameters parm;
    parm.open(filename); 
    libraryInfo.load(libraryInfoFilename);
    if (!configurationFilename.empty()) {
      if (!config.load(configurationFilename)) {
        exceptions.printError("Could not open configuration file " + configurationFilename);
      } else {
	loadPackage("STANDARD", "STD", "ALL");
      }
    }
    transform(library.begin(), library.end(), library.begin(), toupper);
    parm.selectFile(parameters::SOURCE_FILE);
    parm.println("#include \"" + parm.getFileName(parameters::HEADER_FILE) + "\"");
    namespaceStart(parm, library);
    parm.selectFile(parameters::HEADER_FILE);
    std::string ifndefName = library + "_" + parm.baseName(filename) + "_HPP";
    parm.println("#ifndef " + ifndefName);
    parm.println("#define " + ifndefName);
    parm.println("");
    parm.println("#include <string.h>");
    parm.println("#include \"systemc.h\"");
    parm.println("#include \"vhdl.h\"");
    parm.println("#include \"standard.hpp\"");
    parse(parm, designFile, library);
    parm.selectFile(parameters::HEADER_FILE);
    parm.println("#endif");
    parm.selectFile(parameters::SOURCE_FILE);
    namespaceEnd(parm);
    parm.close();
  }

  void SystemC::saveLibraryInfo() {
    libraryInfo.save();
  }

  void SystemC::namespaceStart(parameters& parm, std::string& library) {
    parm.println("");
    parm.println("namespace vhdl {");
    parm.incIndent();
    parm.println("namespace " + library + " {");
    parm.incIndent();
  }

  void SystemC::namespaceEnd(parameters& parm) {
    parm.decIndent();
    parm.println("}");
    parm.decIndent();
    parm.println("}");
  }

  void SystemC::parse(parameters& parm, ast::DesignFile& designFile, std::string& library) {
    functionStart("parse(library = " + library + ")");
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause, true);
    }
    if (library != "STD") {
      parm.println("#include \"standard.h\"");
    }
    namespaceStart(parm, library);
    if (library != "STD") {
      parm.println("STD::STANDARD STD_STANDARD;");
    }
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause, false);
      packageDeclaration(parm, it.module.package, library);
      interfaceDeclaration(parm, it.module.interface, library);
      implementationDeclaration(parm, it.module.implementation, library);
    }
    namespaceEnd(parm);
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
	  parser::DesignFile parserDesignFile;
	  parserDesignFile.parse(filename);
	  parameters p;
          p.setQuiet(true);
	  parse(p, parserDesignFile, library);
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
    ast::ObjectValueContainer type(ast::ENUMERATION, name);
    std::string enumList =
      listToString(parm, t->enumerations, ", ",
                   [&](ast::EnumerationElement& e){
                     std::string s = "";
                     if (e.identifier) {
                       s = e.identifier->toString();
                       database.add(ast::ENUM, s, type);
                     } else if (e.character) {
                       std::string name = e.character->toString();
                       database.add(ast::ENUM, name, type); 
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
    */
    std::string valueName = name + "_value";
    std::string s = std::to_string(size);
    parm.println("enum " + enumName + " {" + enumList + "};");
    parm.println("struct " + valueName + " {");
    parm.incIndent();
    parm.println("EnumerationElement<" + enumName + "> array[" + s + "] {" + structList + "};");
    parm.decIndent();
    parm.println("};");
    parm.println("template<typename T=" + enumName+ ", class E=" + valueName + ", int N=" + s + ">");
    parm.println("using " + name + " = Enumeration<T, E, N>;");
    return ast::ObjectValueContainer(ast::ENUMERATION, name);
  }

  /*
  vhdl:
  type test_t is range 1 to 20;
  */
  ast::ObjectValueContainer SystemC::numberType(parameters& parm, ast::SimpleIdentifier* identifier,
						ast::NumberType* t) {
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
    functionStart("arrayType");
    assert(t); 
    std::string name = identifier->toString(true);
    ast::ObjectValueContainer subtypeValue;
    std::string subtypeName = subtypeIndication(parm, subtypeValue, name, t->type);
    printArrayType(parm, name, t->definition, subtypeName);
    ast::ObjectValueContainer value(ast::ARRAY);
    value.setSubtype(subtypeValue);
    functionEnd("arrayType");
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
    parm.println("using " + name + "_type = decltype(" + left + ");"); 
    parm.println("vhdl_range_type(" + name + ", " + left + ", " + right + ");");
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
    std::string enumList = listToString(parm, p->elements.list, ", ",
                                 [&](ast::PhysicalElement& e){
                                   std::string unit = e.unit->toString(true); 
                                   database.add(ast::ENUM, unit);
                                   return unit;
                                 });
    std::string enumName = name + "_enum";
    parm.println("enum " + enumName + " {" + enumList + "};");
    int size = 0;
    std::string baseUnitName;
    std::string upperUnitName;
    std::string translateList =
      listToString(parm, p->elements.list, ", ",
		   [&](ast::PhysicalElement& e){
		     size++;
		     bool baseUnit = (e.physical == NULL);
		     std::string u = e.unit->toString(true);
		     if (baseUnit) {baseUnitName = u;};
		     upperUnitName = u;
		     std::string number = baseUnit ? "1" : e.physical->number->toString(); 
		     std::string unit = baseUnit ? u : e.physical->unit->toString(true);
		     return "{" + u + ", " + number + ", " + unit + "}";
		   });
    std::string unitName = name + "_unit";
    std::string unitNameList = listToString(parm, p->elements.list, ", ",
					    [&](ast::PhysicalElement& e){
					      return "\"" + e.unit->toString() + "\""; 
					    });
    parm.println("struct " + unitName + " {");
    parm.incIndent();
    {
      parm.println("std::string toString(" + enumName + " e) {");
      parm.incIndent();
      parm.println("static std::string translate[" + std::to_string(size) + "] = {" + unitNameList + "};");
      parm.println("return translate[e];");
      parm.decIndent();
      parm.println("}");
    }
    parm.decIndent();
    parm.println("};");
    std::string valueName = name + "_value";
    parm.println("struct " + valueName + " {");
    parm.incIndent();
    {
      std::string x = std::to_string(size);
      parm.println("const int size = " + x + ";");
      parm.println("const PhysicalElement<" + enumName + ", decltype(" + left + ")> array[" + x + "] {" + translateList + "};");
    }
    parm.decIndent();
    parm.println("};");
    std::string typeName = name + "_type";
    std::string rangeName = "range_" + name;
    std::string declType = "decltype(" + left + ")";
    parm.println("using " + typeName + " = Physical<" + declType + ", " + enumName + ">;"); 
    parm.println("struct " + rangeName + " {" +
		 typeName + " left = {" + left + ", " + baseUnitName + "}; " +
		 typeName + " right = {" + right + ", " + upperUnitName + "};};");
    parm.println("template <class RANGE = " + rangeName +
		 ", typename N = " + declType +
		 ", typename T = " + enumName +
		 ", class E = " + valueName +
		 ", class S = " + unitName + ">");
    parm.println("using " + name + " = PhysicalType<RANGE, N, T, E, S>;");

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
    parm.println("struct " + rangeName + " {");
    parm.incIndent();
    parm.println(typeName + "_type left = " + left + ";");
    parm.println(typeName + "_type right = " + right + ";");
    parm.decIndent();
    parm.println("};");
    parm.println("template<class RANGE = " + rangeName + ">");
    parm.println("using " + name + " = " + typeName + "<RANGE>;");
  }
  
  void SystemC::printArrayType(parameters& parm, std::string& name, ast::ArrayDefinition* r, std::string& subtype) {
    assert(r);
    if (r->range) {
      std::string left, right;
      ast::ObjectValueContainer type(ast::INTEGER);
      rangeToString(r->range, left, right, type);
      parm.println("vhdl_array_type(" + name + ", " + left + ", " + right + ", " + subtype + "<>);");
    } else if (r->subtype) {
      std::string id = r->subtype->identifier->toString(true);
      parm.println("vhdl_array_type(" + name + ", " + id + "<>::LEFT(), " + id + "<>::RIGHT(), " + subtype + "<>);");
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
      exceptions.printError("Could not find type \"" + typeName + "\"", &t->name->text);
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
	if (database.globalName(type, ast::TYPE)) {
	  if (id == ast::SIGNAL) {
	    type = "sc_signal<" + type + ">";
	  }
	  s = type + " " + name;
	  if (initialization && init.size() > 0) {
	    s += " = " + init;
	  }
	} else {
	  exceptions.printError("Could not find definition of type " + type);
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
      parm.println(s + ";");
    }
  }

  void SystemC::packageDeclaration(parameters& parm, ast::Package* package, std::string& library) {
    if (package) {
      std::string name = package->name->toString(true);
      ast::ObjectType type = package->body ? ast::PACKAGE_BODY : ast::PACKAGE;
      functionStart("packageDeclaration(library = " + library +
		    ", packet = " + name + ", type = " + toString(type) + ")");
      topHierarchyStart(parm, library, name, type, filename);
      if (type == ast::PACKAGE) {
        parm.println("");
        parm.println("SC_PACKAGE(" + name + ") {");
        parm.incIndent();
	declarations(parm, package->declarations, false);
        parm.decIndent();
        parm.println("};");
      } else {
        parameters::FileSelect file_select = parm.selectFile(parameters::SOURCE_FILE);
	declarations(parm, package->declarations, true);
	parm.selectFile(file_select);
      }
      topHierarchyEnd(parm, (type == ast::PACKAGE));
      functionEnd("packageDeclaration");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    if (interface) {
      functionStart("interfaceDeclaration");
      std::string name = interface->name->toString(true);
      const ast::ObjectType type = ast::ENTITY;
      topHierarchyStart(parm, library, name, type, filename);
      parm.println("");
      parm.println("SC_INTERFACE(" + name + ") {");
      parm.println("public:");
      parm.incIndent();
      if (interface->generics) {
        parm.println("// Generics");
        parm.println(interfaceListToString(parm, interface->generics, "; ", false) + ";");
      }
      if (interface->ports) {
	auto func = [&](std::string& type, ast::ObjectType id,
			ast::ObjectDeclaration::Direction direction) {
          std::string t = type;
          database.globalName(t, ast::TYPE);
          switch (direction) {
	  case ast::ObjectDeclaration::IN: return "sc_in<" + t + ">";
	  case ast::ObjectDeclaration::OUT: 
	  case ast::ObjectDeclaration::INOUT: 
	  case ast::ObjectDeclaration::BUFFER: return "sc_out<" + t + ">";
	  }
	  return t;
	};
        parm.println("// Ports");
        parm.println(interfaceListToString(parm, interface->ports, "; ", false, func) + ";");
      }
      parm.decIndent();
      parm.println("};");
      topHierarchyEnd(parm, true);
      functionEnd("interfaceDeclaration");
    }
  }

  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library) {
    if (implementation) {
      functionStart("implementationDeclaration");
      std::string name = implementation->name->toString(true);
      const ast::ObjectType type = ast::ARCHITECTURE;
      topHierarchyStart(parm, library, name, type, filename);
      std::string constructor = "SC_CTOR(" + name + ") {init();}";
      defineObject(parm, name, "SC_MODULE", NULL,
                   &implementation->declarations,
                   &implementation->concurrentStatements,
                   [&](parameters& parm){},
		   [&](parameters& parm){},
                   &constructor);
      topHierarchyEnd(parm, false);
      functionEnd("implementationDeclaration");
    }
  }

  
}

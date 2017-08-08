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

  SystemC::SystemC(bool verbose) : debug("SystemC") {
    this->verbose |= verbose; 
  }

  void SystemC::generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename,
                         bool standardPackage) {
    debug.functionStart("SystemC");
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
    if (!standardPackage) {parm.println("#include \"standard.h\"");}
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
    debug.functionStart("parse(library = " + library + ")");
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause, true);
    }
    namespaceStart(parm, library);
    for (ast::DesignUnit& it : designFile.designUnits.list) {
      includes(parm, it.module.contextClause, false);
      packageDeclaration(parm, it.module.package, library);
      interfaceDeclaration(parm, it.module.interface, library);
      implementationDeclaration(parm, it.module.implementation, library);
    }
    namespaceEnd(parm);
    debug.functionEnd("parse");
  }

  void SystemC::parsePackage(parameters& parm, std::string name, std::string library) {
    debug.functionStart("parsePackage(library = " + library + ", name = " + name + ")");
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
    debug.functionEnd("parsePackage");
  }
  


  void SystemC::rangeToString(ast::RangeType* r, std::string& left, std::string& right, ast::ObjectValueContainer& expectedType) {
    assert(r);
    ast::ObjectValueContainer actualType;
    a_expression.getType(r->left, expectedType, actualType);
    left = a_expression.toString(r->left, actualType);
    right = a_expression.toString(r->right, actualType);
  }

  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    std::string left, right;
    ast::ObjectValueContainer type(ast::ObjectValue::NUMBER);
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
    ast::ObjectValueContainer type(ast::ObjectValue::NUMBER);
    ast::ObjectValueContainer physical_type(ast::ObjectValue::PHYSICAL, name);
    rangeToString(r, left, right, type);
    std::string enumList = listToString(parm, p->elements.list, ", ",
                                 [&](ast::PhysicalElement& e){
                                   std::string unit = e.unit->toString(true); 
                                   a_database.add(ast::ObjectType::ENUM, unit, physical_type);
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

  void SystemC::packageDeclaration(parameters& parm, ast::Package* package, std::string& library) {
    if (package) {
      std::string name = package->name->toString(true);
      ast::ObjectType type = package->body ? ast::ObjectType::PACKAGE_BODY : ast::ObjectType::PACKAGE;
      debug.functionStart("packageDeclaration(library = " + library +
		    ", packet = " + name + ", type = " + toString(type) + ")");
      topHierarchyStart(parm, library, name, type, filename);
      if (type == ast::ObjectType::PACKAGE) {
        parm.println("");
        parm.println("SC_PACKAGE(" + name + ") {");
        parm.incIndent();
        parameters::Area area = parm.area;
        parm.setArea(parameters::Area::DECLARATION);
	declarations(parm, package->declarations);
        parm.setArea(area);
        parm.decIndent();
        parm.println("};");
        parm.decIndent();
        parm.println("}");
        parm.println("extern " + library + "::" + name + " " + library + "_" + name + ";");
        parm.println("namespace " + library + " {");
        parm.incIndent();
      } else {
        parameters::FileSelect file_select = parm.selectFile(parameters::SOURCE_FILE);
	parameters::Area area = parm.area;
        parm.setArea(parameters::Area::IMPLEMENTATION);
	declarations(parm, package->declarations);
        parm.decIndent();
        parm.println("}");
        parm.println(library + "::" + name + " " + library + "_" + name + ";");
        parm.println("namespace " + library + " {");
        parm.incIndent();
	parm.setArea(area);
        parm.selectFile(file_select);
      }
      topHierarchyEnd(parm, (type == ast::ObjectType::PACKAGE));
      debug.functionEnd("packageDeclaration");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    if (interface) {
      debug.functionStart("interfaceDeclaration");
      std::string name = interface->name->toString(true);
      const ast::ObjectType type = ast::ObjectType::ENTITY;
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
          switch (direction) {
	  case ast::ObjectDeclaration::Direction::IN: return "sc_in<" + type + ">";
	  case ast::ObjectDeclaration::Direction::OUT: 
	  case ast::ObjectDeclaration::Direction::INOUT: 
	  case ast::ObjectDeclaration::Direction::BUFFER: return "sc_out<" + type + ">";
	  }
	  return type;
	};
        parm.println("// Ports");
        parm.println(interfaceListToString(parm, interface->ports, "; ", false, func) + ";");
      }
      parm.decIndent();
      parm.println("};");
      topHierarchyEnd(parm, true);
      debug.functionEnd("interfaceDeclaration");
    }
  }

  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library) {
    if (implementation) {
      debug.functionStart("implementationDeclaration");
      std::string name = implementation->name->toString(true);
      const ast::ObjectType type = ast::ObjectType::ARCHITECTURE;
      topHierarchyStart(parm, library, name, type, filename);
      if (!a_database.localize(library, name, ast::ObjectType::ENTITY)) {
        exceptions.printError("Could not find " + ast::toString(ast::ObjectType::ENTITY) + " " +
                              library + "." + name, &implementation->name->text);
        if (verbose) {
          a_database.print(library);
        }
      }
      auto bodyCallback = [&](parameters& parm) {
        parm.println("SC_CTOR(" + name + ") {init();}");
      };
      defineObject(parm, true, name, "SC_MODULE", NULL,
                   &implementation->declarations,
                   &implementation->concurrentStatements,
                   bodyCallback,
		   [&](parameters& parm){},
                   false);
      topHierarchyEnd(parm, false);
      debug.functionEnd("implementationDeclaration");
    }
  }

  
}

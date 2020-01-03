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
    debug.functionStart("generate");
    a_filename = designFile.filename;
    parameters parm;
    parm.open(a_filename, library); 
    libraryInfo.load(libraryInfoFilename);
    if (!configurationFilename.empty()) {
      if (!config.load(configurationFilename)) {
        exceptions.printError("Could not open configuration file " + configurationFilename);
      } else {
        if (designFile.IsLanguage(ast::DesignFile::LanguageType::VHDL)) {
          loadPackage(parm, "STANDARD", "STD", "ALL");
        }
      }
    }
    if (!standardPackage) {parm.addTop("#include <standard.h>");}
    parse(parm, designFile, library);
    parm.close();
    debug.functionEnd("generate");
  }

  void SystemC::saveLibraryInfo() {
    libraryInfo.save();
  }

  void SystemC::parse(parameters& parm, ast::DesignFile& designFile, std::string& library) {
    debug.functionStart("parse(designFile = " + designFile.filename + ", library = " + library + ")");
    if (!designFile.designUnits.list.empty()) {
      for (ast::DesignUnit& it : designFile.designUnits.list) {
        includes(parm, it.module.contextClause, true);
      }
      bool unit_found = false;
      for (ast::DesignUnit& it : designFile.designUnits.list) {
        debug.debug("Parsing design unit");
        includes(parm, it.module.contextClause, false);
        if (it.module.package) {packageDeclaration(parm, it.module.package, library); unit_found = true;}
        if (it.module.interface) {interfaceDeclaration(parm, it.module.interface, library); unit_found = true;}
        if (it.module.implementation) {implementationDeclaration(parm, it.module.implementation, library); unit_found = true;}
      }
      if (!unit_found) {
        exceptions.printWarning("Did not find any design units in file " + designFile.filename);
      }
    } else {
      exceptions.printWarning("Did not find anything in file " + designFile.filename); 
    }
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
	if (filename != a_filename) {
          std::string hpp_filename = parm.replaceFileExtension(filename, ".hpp");
	  parm.addTop("#include \"" + hpp_filename + "\"");
          parm.setPackageName(stdPath, filename);
	  filename = stdPath + "/" + filename;
          parser::DesignFile parserDesignFile;
	  parserDesignFile.parse(filename);
	  parameters p;
          p.setQuiet(true);
          p.parse_declarations_only = true;
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
    debug.functionEnd("parsePackage(name = " + name + ")");
  }
  


  void SystemC::rangeToString(ast::RangeType* r, std::string& left, std::string& right, ast::ObjectValueContainer& expectedType) {
    debug.functionStart("rangeToString(expectedType = " + expectedType.toString() + ")");
    assert(r);
    if (r->range_direction_type) {
      a_expression.CollectAllReturnTypes(r->range_direction_type->left, expectedType);
      left = a_expression.toString(r->range_direction_type->left, expectedType);
      right = a_expression.toString(r->range_direction_type->right, expectedType);
    } else if (r->range_attribute_type) {
      
    } else {
      assert(false);
    }
    debug.functionEnd("rangeToString");
  }

  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    debug.functionStart("printRangeType");
    std::string left, right;
    ast::ObjectValueContainer type(ast::ObjectValue::NUMBER);
    rangeToString(r, left, right, type);
    parm.addTop("using " + name + " = Range<decltype(" + left + ")>;");
    PrintFactory(parm, name, r, NULL, ast::ObjectValue::NUMBER);
    debug.functionEnd("printRangeType");
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
    parm.addClassContents("enum " + enumName + " {" + enumList + "};");
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
    parm.addClassContents("struct " + unitName + " {");
    {
      parm.addClassContents("std::string toString(" + enumName + " e) {");
      parm.addClassContents("static std::string translate[" + std::to_string(size) + "] = {" + unitNameList + "};");
      parm.addClassContents("return translate[e];");
      parm.addClassContents("}");
    }
    parm.addClassContents("};");
    std::string valueName = name + "_value";
    parm.addClassContents("struct " + valueName + " {");
    {
      std::string x = std::to_string(size);
      parm.addClassContents("const int size = " + x + ";");
      parm.addClassContents("const PhysicalElement<" + enumName + ", decltype(" + left + ")> array[" + x + "] {" + translateList + "};");
    }
    parm.addClassContents("};");
    std::string typeName = name + "_type";
    std::string declType = "decltype(" + left + ")";
    parm.addClassContents("using " + typeName + " = Physical<" + declType + ", " + enumName + ">;"); 
    parm.addClassContents("using " + name + " = PhysicalType<" + declType + ", " + enumName + ", " + valueName + ", " + unitName + ">;");
    auto f = [&](parameters& parm, std::string& l, std::string& r) {
      l = "{" + left + ", " + baseUnitName + "}";
      r = "{" + right + ", " + upperUnitName + "}";
    };
    PrintFactory(parm, name, f);
  }

  std::string SystemC::ObjectName(ast::ObjectType type, const std::string& name) {
    std::string type_name = ast::toString(type);
    type_name[0] = toupper(type_name[0]);
    return type_name + "_" + name;
  }
  
  void SystemC::packageDeclaration(parameters& parm, ast::Package* package, std::string& library) {
    ast::ObjectType type = package->body ? ast::ObjectType::PACKAGE_BODY : ast::ObjectType::PACKAGE;
    if (!parm.parse_declarations_only || type == ast::ObjectType::PACKAGE) { 
      std::string name = package->name->toString(true);
      debug.functionStart("packageDeclaration(library = " + library +
                          ", packet = " + name + ", type = " + toString(type) + ")");
      topHierarchyStart(parm, library, name, type, a_filename);
      parm.package_contains_function = false;
      std::string derived_name = (type == ast::ObjectType::PACKAGE_BODY) ? ObjectName(ast::ObjectType::PACKAGE, name) : "";
      auto createDefinition = [&](parameters& parm) {
        parm.setClassConstructorDescription(ObjectName(type, name));
      };
      auto createBody = [&](parameters& parm) {
      };
      bool init_enable = (type == ast::ObjectType::PACKAGE_BODY);
      defineObject(parm, true, name, type, derived_name, NULL,
                   &package->declarations, NULL, createBody, createDefinition, false, true);
      bool declare_object = type == ast::ObjectType::PACKAGE_BODY || !parm.package_contains_function;
      if (declare_object) {
        parm.addClassContents("using " + name + " = " + ObjectName(type, name) + ";");
      }
      if (declare_object) {
        std::string declaration = library + "::" + name + " " + library + "_" + name + ";";
        parm.addBottom("extern " + declaration);
        parm.addBottom(declaration);
      }
      topHierarchyEnd(parm, (type == ast::ObjectType::PACKAGE));
      debug.functionEnd("packageDeclaration");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    debug.functionStart("interfaceDeclaration");
    std::string name = interface->name->toString(true);
    const ast::ObjectType type = ast::ObjectType::ENTITY;
    topHierarchyStart(parm, library, name, type, a_filename);
    parm.newClass("SC_MODULE(" + ast::toString(type) + "_" + name + ")");
    if (interface->generics) {
      assert(false);
      // Not implemented yet
      // parm.println(interfaceListToString(parm, interface->generics, "; ", false) + ";");
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
      parm.addClassContents("// Ports");
      parm.addClassContents(interfaceListToString(parm, interface->ports, "; ", false, func) + ";");
    }
    parm.endClass();
    topHierarchyEnd(parm, true);
    debug.functionEnd("interfaceDeclaration");
  }
  
  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library) {
    if (implementation) {
      debug.functionStart("implementationDeclaration");
      std::string name = implementation->name->toString(true);
      const ast::ObjectType type = ast::ObjectType::ARCHITECTURE;
      topHierarchyStart(parm, library, name, type, a_filename);
      if (!a_database.localize(library, name, ast::ObjectType::ENTITY)) {
        exceptions.printError("Could not find " + ast::toString(ast::ObjectType::ENTITY) + " " +
                              library + "." + name, &implementation->name->text);
        if (verbose) {
          a_database.print(library);
        }
      }
      auto declaration_callback = [&](parameters& parm) {
        parm.setClassConstructorDescription(ObjectName(type, name) + "(const sc_module_name& name)");
      };
      defineObject(parm, true, name, type, ast::toString(ast::ObjectType::ENTITY) + "_" + name, NULL,
                   &implementation->declarations,
                   &implementation->concurrentStatements,
                   [&](parameters& parm){},
                   declaration_callback,
                   false, true);
      topHierarchyEnd(parm, false);
      debug.functionEnd("implementationDeclaration");
    }
  }

  
}

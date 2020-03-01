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
    a_verbose |= verbose; 
  }

  void SystemC::generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename,
                         bool standardPackage) {
    debug.functionStart("generate(library = " + library + ")");
    a_filename = designFile.filename;
    parameters parm(library, a_verbose);
    parm.open(a_filename); 
    libraryInfo.load(libraryInfoFilename);
    if (!configurationFilename.empty()) {
      if (!config.load(configurationFilename)) {
        exceptions.printError("Could not open configuration file " + configurationFilename);
      } else {
        if (designFile.IsLanguage(ast::DesignFile::LanguageType::VHDL)) {
	  if (!standardPackage) {
	    loadPackage(parm, "STANDARD", "STD", "ALL");
	  }
	}
      }
    }
    if (!standardPackage) {parm.addInclude("#include <standard.h>");}
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
      std::string config_file = stdPath + "/" + libraryInfoFilename;
      c.load(config_file);
      std::string filename = c.find("package", name);
      if (!filename.empty()) {
	if (filename != a_filename) {
          std::string header_filename = NameConverter::getHeaderFileName(library, filename);
	  parm.addInclude("#include \"" + header_filename + "\"");
          parm.setPackageName(stdPath, filename);
	  filename = stdPath + "/" + filename;
          parser::DesignFile parserDesignFile;
	  parserDesignFile.parse(filename);
	  parameters p(library, a_verbose);
          p.setQuiet(true);
          p.parse_declarations_only = true;
	  parse(p, parserDesignFile, library);
	} else {
	  exceptions.printError("Could not find package \"" + name + "\" in file " + filename +
				", when using config file: " + config_file);
	}
      } else {
        exceptions.printError("Could not resolve filename of package \"" + name + "\"");
      }
    } else {
      exceptions.printError("Could not find library \"" + library + "\" is [hdl] section of config file");
    }
    debug.functionEnd("parsePackage(name = " + name + ")");
  }
  


  void SystemC::rangeToString(parameters& parm, ast::RangeType* r, std::string& left, std::string& right, ast::ObjectValueContainer& expectedType) {
    debug.functionStart("rangeToString(expectedType = " + expectedType.toString() + ")");
    assert(r);
    if (r->range_direction_type) {
      a_expression.collectAllReturnTypes(parm, r->range_direction_type->left, expectedType);
      left = a_expression.toString(parm, r->range_direction_type->left, expectedType);
      right = a_expression.toString(parm, r->range_direction_type->right, expectedType);
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
    rangeToString(parm, r, left, right, type);
    parm.addClassContents("using " + name + " = vhdl::Range<decltype(" + left + ")>;");
    PrintFactory(parm, name, r, NULL, ast::ObjectValue::NUMBER);
    debug.functionEnd("printRangeType");
  }

  void SystemC::printPhysicalType(parameters& parm, std::string& name, ast::NumberType* n) {
    debug.functionStart("printPhysicalType");
    assert(n);
    ast::RangeType* r = n->range;
    assert(r);
    ast::PhysicalType* p = n->physical;
    assert(p);
    std::string left, right;
    ast::ObjectValueContainer type(ast::ObjectValue::NUMBER);
    ast::ObjectValueContainer physical_type(ast::ObjectValue::PHYSICAL, name);
    rangeToString(parm, r, left, right, type);
    std::string enumList = listToString(parm, p->elements.list, ", ",
                                 [&](ast::PhysicalElement& e){
                                   std::string unit = e.unit->toString(true); 
                                   parm.addObject(ast::ObjectType::ENUM, unit, physical_type);
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
      parm.addClassContents("const vhdl::PhysicalElement<" + enumName + ", decltype(" + left + ")> array[" + x + "] {" + translateList + "};");
    }
    parm.addClassContents("};");
    std::string typeName = name + "_type";
    std::string declType = "decltype(" + left + ")";
    parm.addClassContents("using " + typeName + " = vhdl::Physical<" + declType + ", " + enumName + ">;"); 
    parm.addClassContents("using " + name + " = vhdl::PhysicalType<" + declType + ", " + enumName + ", " + valueName + ", " + unitName + ">;");
    auto f = [&](parameters& parm, std::string& l, std::string& r) {
      l = "{" + left + ", " + baseUnitName + "}";
      r = "{" + right + ", " + upperUnitName + "}";
    };
    PrintFactory(parm, name, f);
    debug.functionEnd("printPhysicalType");
  }

  void SystemC::packageDeclaration(parameters& parm, ast::Package* package, std::string& library) {
    ast::ObjectType type = package->body ? ast::ObjectType::PACKAGE_BODY : ast::ObjectType::PACKAGE;
    if (!parm.parse_declarations_only || type == ast::ObjectType::PACKAGE) { 
      std::string name = package->name->toString(true);
      debug.functionStart("packageDeclaration(library = " + library +
                          ", packet = " + name + ", type = " + toString(type) + ")");
      topHierarchyStart(parm, library, name, type, a_filename);
      parm.package_contains_function = false;
      // bool declare_object = type == ast::ObjectType::PACKAGE_BODY || !parm.package_contains_function;
      std::string class_name = NameConverter::objectName(type, name);
      auto callback =
	[&](parameters& parm)
	{
	  parm.setClassConstructorDescription(class_name + "()");
	  if (type == ast::ObjectType::PACKAGE_BODY) {
	    parm.addDerivedClass(NameConverter::objectName(ast::ObjectType::PACKAGE, name));
	  }
	  if (type == ast::ObjectType::PACKAGE) {
	    parm.addClassTrailer("using " + name + " = " + class_name + ";");
      	  }
	};
      std::string class_description = "struct " + class_name;
      defineObject(parm, true, name, type, "", &class_description, NULL,
                   &package->declarations, NULL, callback, false, true);
      if (type == ast::ObjectType::PACKAGE_BODY) {
	std::string l = NameConverter::toUpper(library);
	std::string declaration = l + "::" + class_name + " " + l + "_" + name + ";";
	parm.addBottom("extern " + declaration);
	parm.addImplementationContents(declaration, __FILE__, __LINE__);
      }
      topHierarchyEnd(parm, &package->name->text, (type == ast::ObjectType::PACKAGE));
      debug.functionEnd("packageDeclaration");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    debug.functionStart("interfaceDeclaration");
    std::string name = interface->name->toString(true);
    const ast::ObjectType type = ast::ObjectType::ENTITY;
    std::string class_name = NameConverter::objectName(type, name);
    topHierarchyStart(parm, library, name, type, a_filename);
    auto callback =
      [&](parameters& parm) {
	if (interface->generics) {
	  assert(false);
	  // Not implemented yet
	  // parm.println(interfaceListToString(parm, interface->generics, "; ", false) + ";");
	}
	if (interface->ports) {
	  auto func = [&](std::string& type, ast::ObjectType id,
			  ast::ObjectDeclaration::Direction direction) {
			switch (direction) {
			case ast::ObjectDeclaration::Direction::IN: return "vhdl::interface<sc_in<" + type + ">, " + type + ">";
			case ast::ObjectDeclaration::Direction::OUT: 
			case ast::ObjectDeclaration::Direction::INOUT: 
			case ast::ObjectDeclaration::Direction::BUFFER: return "vhdl::interface<sc_out<" + type + ">, " + type + ">";
			default: {}
			}
			return type;
		      };
	  parm.addClassContents("// Ports");
	  parm.addClassContents(interfaceListToString(parm, interface->ports, "; ", false, func) + ";");
	}
	parm.setClassConstructorDescription(class_name + "(const sc_module_name& name)");
	parm.addClassConstructorInitializer("sc_module(name)");
	parm.addClassContents("SC_HAS_PROCESS(" + class_name + ");");
      };
    std::string class_description = "SC_MODULE(" + class_name + ")";
    defineObject(parm, true, name, type, "", &class_description, NULL,
		 NULL,
		 NULL,
		 callback,
		 false, true);
    topHierarchyEnd(parm, &interface->name->text, true);
    debug.functionEnd("interfaceDeclaration");
  }
  
  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library) {
    if (implementation) {
      debug.functionStart("implementationDeclaration");
      std::string entity_name = implementation->entity_name->toString(true);
      std::string architecture_name = entity_name + "_" + implementation->architecture_name->toString(true);
      const ast::ObjectType type = ast::ObjectType::ARCHITECTURE;
      topHierarchyStart(parm, library, architecture_name, type, a_filename);
      std::shared_ptr<LocalDatabase> object;
      if (!parm.findObject(object, library, entity_name, ast::ObjectType::ENTITY)) {
        exceptions.printError("Could not find " + ast::toString(ast::ObjectType::ENTITY) + " " +
                              library + "." + entity_name, &implementation->entity_name->text);
        if (a_verbose) {
	  parm.printDatabase(library);
        }
      }
      std::string class_name = NameConverter::objectName(type, architecture_name);
      auto callback =
	[&](parameters& parm) {
	  std::string derived_class = NameConverter::objectName(ast::ObjectType::ENTITY, entity_name);
	  parm.addDerivedClass(derived_class);
	  parm.setClassConstructorDescription(class_name + "(const sc_module_name& name)");
	  parm.addClassConstructorInitializer(derived_class + "(name)");
	};
      std::string class_description = "struct " + class_name;
      defineObject(parm, true, architecture_name, type, entity_name, &class_description, NULL,
                   &implementation->declarations,
                   &implementation->concurrentStatements,
                   callback,
                   false, true);
      topHierarchyEnd(parm, &implementation->architecture_name->text);
      debug.functionEnd("implementationDeclaration");
    }
  }

  
}

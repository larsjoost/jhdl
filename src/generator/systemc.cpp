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

  SystemC::SystemC(bool verbose) : m_debug(this) {
    a_verbose |= verbose; 
  }

  void SystemC::generate(ast::DesignFile& designFile, std::string& library, std::string& configurationFilename,
                         bool standardPackage) {
    m_debug.functionStart("generate(library = " + library + ")", false, __FILE__, __LINE__);
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
    m_debug.functionEnd("generate");
  }

  void SystemC::saveLibraryInfo() {
    libraryInfo.save();
  }

  void SystemC::parse(parameters& parm, ast::DesignFile& designFile, std::string& library) {
    m_debug.functionStart("parse(designFile = " + designFile.filename + ", library = " + library + ")",
			  false, __FILE__, __LINE__);
    if (!designFile.designUnits.list.empty()) {
      for (ast::DesignUnit& it : designFile.designUnits.list) {
        includes(parm, it.module.contextClause, true);
      }
      bool unit_found = false;
      for (ast::DesignUnit& it : designFile.designUnits.list) {
        m_debug.debug("Parsing design unit");
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
    m_debug.functionEnd("parse");
  }

  void SystemC::parsePackage(parameters& parm, std::string name, std::string library) {
    m_debug.functionStart("parsePackage(library = " + library + ", name = " + name + ")",
			  false, __FILE__, __LINE__);
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
    m_debug.functionEnd("parsePackage(name = " + name + ")");
  }
  


  ast::ObjectValueContainer SystemC::rangeToString(parameters& parm, ast::RangeType* r, RangeDefinition& range_definition,
						   ast::ObjectValueContainer& expectedType) {
    m_debug.functionStart("rangeToString(expectedType = " + expectedType.toString() + ")",
			  false, __FILE__, __LINE__);
    ast::ObjectValueContainer found_type;
    assert(r);
    if (r->range_direction_type) {
      ast::ObjectValueContainer left_type;
      ast::ObjectValueContainer right_type;
      ExpectedType t(expectedType);
      a_expression.collectUniqueReturnType(parm, r->range_direction_type->left, left_type, &t);
      a_expression.collectUniqueReturnType(parm, r->range_direction_type->right, right_type, &t);
      m_debug.debug("Left type = " + left_type.toString() + ", right type = " + right_type.toString());
      if (left_type != right_type) {
	exceptions.printError("Left range type " + left_type.toString() + " is not the same as right type " +
			      right_type.toString(), r->range_direction_type->left->text);
      }
      range_definition.left = a_expression.toString(parm, r->range_direction_type->left, t);
      range_definition.right = a_expression.toString(parm, r->range_direction_type->right, t);
      if (r->range_direction_type->range_direction) {
	range_definition.ascending = (r->range_direction_type->range_direction->direction == ast::RangeDirection::Direction::TO ? "true" : "false");
      }
      found_type = left_type;
    } else if (r->range_attribute_type) {
      
    } else {
      assert(false);
    }
    m_debug.functionEnd("rangeToString: " + found_type.toString());
    return found_type;
  }

  ast::ObjectValue SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    m_debug.functionStart("printRangeType(name = " + name + ")", false, __FILE__, __LINE__);
    RangeDefinition range_definition;
    ast::ObjectValueContainer expected_type(ast::ObjectValue::NUMBER);
    ast::ObjectValueContainer found_type = rangeToString(parm, r, range_definition, expected_type);
    bool integer_type = found_type.IsValue(ast::ObjectValue::INTEGER);
    std::string t = integer_type ? "int" : "double";
    parm.addClassContents("using " + name + " = vhdl::Range<" + t + ">;", __FILE__, __LINE__);
    printFactory(parm, name, r, NULL, expected_type);
    m_debug.functionEnd("printRangeType: " + found_type.toString());
    return found_type.GetValue();
  }

  void SystemC::printPhysicalType(parameters& parm, std::string& name, ast::NumberType* n) {
    m_debug.functionStart("printPhysicalType", false, __FILE__, __LINE__);
    assert(n);
    ast::RangeType* r = n->range;
    assert(r);
    ast::PhysicalType* p = n->physical;
    assert(p);
    RangeDefinition range_definition;
    ast::ObjectValueContainer type(ast::ObjectValue::NUMBER);
    ast::ObjectValueContainer physical_type(ast::ObjectValue::PHYSICAL, name);
    rangeToString(parm, r, range_definition, type);
    std::string enumList = listToString(parm, p->elements.list, ", ",
                                 [&](ast::PhysicalElement& e){
                                   std::string unit = e.unit->toString(true); 
                                   parm.addObjectValueContainer(ast::ObjectType::ENUM, unit, physical_type);
                                   return unit;
                                 });
    std::string enumName = name + "_enum";
    parm.addClassContents("enum " + enumName + " {" + enumList + "};", __FILE__, __LINE__);
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
    parm.addClassContents("struct " + unitName + " {", __FILE__, __LINE__);
    {
      parm.addClassContents("std::string toString(" + enumName + " e) {", __FILE__, __LINE__);
      parm.addClassContents("static std::string translate[" + std::to_string(size) + "] = {" + unitNameList + "};", __FILE__, __LINE__);
      parm.addClassContents("return translate[e];", __FILE__, __LINE__);
      parm.addClassContents("}", __FILE__, __LINE__);
    }
    parm.addClassContents("};", __FILE__, __LINE__);
    std::string valueName = name + "_value";
    parm.addClassContents("struct " + valueName + " {", __FILE__, __LINE__);
    {
      std::string x = std::to_string(size);
      parm.addClassContents("const int size = " + x + ";", __FILE__, __LINE__);
      parm.addClassContents("const vhdl::PhysicalElement<" + enumName + ", decltype(" + range_definition.left + ")> array[" + x + "] {" + translateList + "};", __FILE__, __LINE__);
    }
    parm.addClassContents("};", __FILE__, __LINE__);
    std::string typeName = name + "_type";
    std::string declType = "decltype(" + range_definition.left + ")";
    parm.addClassContents("using " + typeName + " = vhdl::Physical<" + declType + ", " + enumName + ">;", __FILE__, __LINE__); 
    parm.addClassContents("using " + name + " = vhdl::PhysicalType<" + declType + ", " + enumName + ", " + valueName + ", " + unitName + ">;", __FILE__, __LINE__);
    auto f = [&](parameters& parm, RangeDefinition& r, RangeDefinition& s) {
      r.left = "{" + range_definition.left + ", " + baseUnitName + "}";
      r.right = "{" + range_definition.right + ", " + upperUnitName + "}";
    };
    printFactoryDefinition(parm, name, f);
    m_debug.functionEnd("printPhysicalType");
  }

  void SystemC::packageDeclaration(parameters& parm, ast::Package* package, std::string& library) {
    ast::ObjectType type = package->body ? ast::ObjectType::PACKAGE_BODY : ast::ObjectType::PACKAGE;
    if (!parm.parse_declarations_only || type == ast::ObjectType::PACKAGE) { 
      std::string name = package->name->toString(true);
      m_debug.functionStart("packageDeclaration(library = " + library +
                          ", packet = " + name + ", type = " + toString(type) + ")",
			    false, __FILE__, __LINE__);
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
      m_debug.functionEnd("packageDeclaration");
    }
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    m_debug.functionStart("interfaceDeclaration", false, __FILE__, __LINE__);
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
	  parm.addClassContents("// Ports", __FILE__, __LINE__);
	  parm.addClassContents(interfaceListToString(parm, interface->ports, "; ", false, func) + ";", __FILE__, __LINE__);
	}
	parm.setClassConstructorDescription(class_name + "(const sc_module_name& name)");
	parm.addClassConstructorInitializer("sc_module(name)");
	parm.addClassContents("SC_HAS_PROCESS(" + class_name + ");", __FILE__, __LINE__);
      };
    std::string class_description = "SC_MODULE(" + class_name + ")";
    defineObject(parm, true, name, type, "", &class_description, NULL,
		 NULL,
		 NULL,
		 callback,
		 false, true);
    topHierarchyEnd(parm, &interface->name->text, true);
    m_debug.functionEnd("interfaceDeclaration");
  }
  
  void SystemC::implementationDeclaration(parameters& parm, ast::Implementation* implementation, std::string& library) {
    if (implementation) {
      m_debug.functionStart("implementationDeclaration", false, __FILE__, __LINE__);
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
      m_debug.functionEnd("implementationDeclaration");
    }
  }

  
}

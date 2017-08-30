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
    transform(library.begin(), library.end(), library.begin(), toupper);
    parm.selectFile(parameters::FileSelect::SOURCE);
    parm.SetArea(parameters::Area::TOP);
    parm.println("#include \"" + parm.getFileName(parameters::FileSelect::HEADER) + "\"");
    parm.println("namespace vhdl {");
    parm.selectFile(parameters::FileSelect::HEADER);
    parm.SetArea(parameters::Area::TOP);
    std::string ifndefName = library + "_" + parm.baseName(filename) + "_HPP";
    parm.println("#ifndef " + ifndefName);
    parm.println("#define " + ifndefName);
    parm.println("");
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
    parm.SetArea(parameters::Area::TOP);
    parm.println("#include <string.h>");
    parm.println("#include \"systemc.h\"");
    parm.println("#include \"vhdl.h\"");
    if (!standardPackage) {parm.println("#include \"standard.h\"");}
    parse(parm, designFile, library);
    parm.selectFile(parameters::FileSelect::HEADER);
    parm.println("#endif");
    parm.selectFile(parameters::FileSelect::SOURCE);
    parm.println("}");
    parm.close();
  }

  void SystemC::saveLibraryInfo() {
    libraryInfo.save();
  }

  void SystemC::namespaceStart(parameters& parm, std::string& library) {
    parm.println("namespace vhdl { namespace " + library + " {");
  }

  void SystemC::namespaceEnd(parameters& parm) {
    parm.println("}}");
  }

  void SystemC::parse(parameters& parm, ast::DesignFile& designFile, std::string& library) {
    debug.functionStart("parse(designFile = " + designFile.filename + ", library = " + library + ")");
    parm.SetArea(parameters::Area::TOP);
    if (!designFile.designUnits.list.empty()) {
      for (ast::DesignUnit& it : designFile.designUnits.list) {
        includes(parm, it.module.contextClause, true);
      }
      parm.SetArea(parameters::Area::TOP);
      bool unit_found = false;
      for (ast::DesignUnit& it : designFile.designUnits.list) {
        debug.debug("Parsing design unit");
        parm.SetArea(parameters::Area::TOP);
        includes(parm, it.module.contextClause, false);
        parm.SetArea(parameters::Area::TOP);
        namespaceStart(parm, library);
        if (it.module.package) {packageDeclaration(parm, it.module.package, library); unit_found = true;}
        if (it.module.interface) {interfaceDeclaration(parm, it.module.interface, library); unit_found = true;}
        if (it.module.implementation) {implementationDeclaration(parm, it.module.implementation, library); unit_found = true;}
        namespaceEnd(parm);
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
	if (filename != this->filename) {
	  parm.println(parameters::Area::TOP, "#include \"" + parm.replaceFileExtension(filename, ".hpp") + "\"");
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
    debug.functionEnd("parsePackage(name = " + name + ")");
  }
  


  void SystemC::rangeToString(ast::RangeType* r, std::string& left, std::string& right, ast::ObjectValueContainer& expectedType) {
    debug.functionStart("rangeToString(expectedType = " + expectedType.toString() + ")");
    assert(r);
    a_expression.CollectAllReturnTypes(r->left, expectedType);
    left = a_expression.toString(r->left, expectedType);
    right = a_expression.toString(r->right, expectedType);
    debug.functionEnd("rangeToString");
  }

  void SystemC::printRangeType(parameters& parm, std::string& name, ast::RangeType* r) {
    debug.functionStart("printRangeType");
    std::string left, right;
    ast::ObjectValueContainer type(ast::ObjectValue::NUMBER);
    rangeToString(r, left, right, type);
    parm.println(parameters::Area::DECLARATION, "using " + name + " = Range<decltype(" + left + ")>;");
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
    {
      parm.println("std::string toString(" + enumName + " e) {");
      parm.println("static std::string translate[" + std::to_string(size) + "] = {" + unitNameList + "};");
      parm.println("return translate[e];");
      parm.println("}");
    }
    parm.println("};");
    std::string valueName = name + "_value";
    parm.println("struct " + valueName + " {");
    {
      std::string x = std::to_string(size);
      parm.println("const int size = " + x + ";");
      parm.println("const PhysicalElement<" + enumName + ", decltype(" + left + ")> array[" + x + "] {" + translateList + "};");
    }
    parm.println("};");
    std::string typeName = name + "_type";
    std::string declType = "decltype(" + left + ")";
    parm.println("using " + typeName + " = Physical<" + declType + ", " + enumName + ">;"); 
    parm.println("using " + name + " = PhysicalType<" + declType + ", " + enumName + ", " + valueName + ", " + unitName + ">;");
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
    std::string name = package->name->toString(true);
    ast::ObjectType type = package->body ? ast::ObjectType::PACKAGE_BODY : ast::ObjectType::PACKAGE;
    debug.functionStart("packageDeclaration(library = " + library +
                        ", packet = " + name + ", type = " + toString(type) + ")");
    topHierarchyStart(parm, library, name, type, filename);
    parm.package_contains_function = false;
    std::string derived_name = (type == ast::ObjectType::PACKAGE_BODY) ? ObjectName(ast::ObjectType::PACKAGE, name) : "";
    auto createDefinition = [&](parameters& parm) {
      parm.println(ObjectName(type, name) + "() {init();}");
    };
    auto createBody = [&](parameters& parm) {
    };
    bool init_enable = (type == ast::ObjectType::PACKAGE_BODY);
    DefineObject(parm, true, name, type, derived_name, NULL,
                 &package->declarations, NULL, createBody, createDefinition, false, true);
    bool declare_object = type == ast::ObjectType::PACKAGE_BODY || !parm.package_contains_function;
    if (declare_object) {
      parm.println("using " + name + " = " + ObjectName(type, name) + ";");
    }
    parm.println("}");
    if (declare_object) {
      std::string declaration = library + "::" + name + " " + library + "_" + name + ";";
      parm.println("extern " + declaration);
      parameters::FileSelect file_select = parm.selectFile(parameters::FileSelect::SOURCE);
      parm.println(declaration);
      parm.selectFile(file_select);
    }
    parm.println("namespace " + library + " {");
    topHierarchyEnd(parm, (type == ast::ObjectType::PACKAGE));
    debug.functionEnd("packageDeclaration");
  }

  void SystemC::interfaceDeclaration(parameters& parm, ast::Interface* interface, std::string& library) {
    debug.functionStart("interfaceDeclaration");
    std::string name = interface->name->toString(true);
    const ast::ObjectType type = ast::ObjectType::ENTITY;
    topHierarchyStart(parm, library, name, type, filename);
    parm.println("");
    parm.println("class " + ast::toString(type) + "_" + name + " {");
    parm.println("public:");
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
    parm.println("};");
    topHierarchyEnd(parm, true);
    debug.functionEnd("interfaceDeclaration");
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
      auto declaration_callback = [&](parameters& parm) {
        std::string initializer_list = parm.ToList(parameters::Area::INITIALIZER_LIST);
        parm.println(ObjectName(type, name) + "(const char* name)" +
                     (initializer_list.empty() ? "" : " : " + initializer_list) + " {init();}");
      };
      parm.println("");
      DefineObject(parm, true, name, type, "sc_module, " + ast::toString(ast::ObjectType::ENTITY) + "_" + name, NULL,
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

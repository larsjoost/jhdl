#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"
#include "database/database.hpp"

namespace generator {

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
      value = ast::ObjectValue::PHYSICAL;
    } else {
      printRangeType(parm, name, t->range);
      value = ast::ObjectValue::INTEGER;
    }
    return ast::ObjectValueContainer(value, name);
  }
  
  ast::ObjectValueContainer SystemC::enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t) {
    assert(t); 
    std::string name = identifier->toString(true);
    ast::ObjectValueContainer type(ast::ObjectValue::ENUMERATION, name);
    int enum_size = 0;
    std::string enumList =
      listToString(parm, t->enumerations, ", ",
                   [&](ast::EnumerationElement& e){
                     std::string s = "";
                     if (e.identifier) {
                       enum_size++;
                       s = e.identifier->toString(true);
                       a_database.add(ast::ObjectType::ENUM, s, type);
                     } else if (e.character) {
                       std::string name = e.character->toString();
                       a_database.add(ast::ObjectType::ENUM, name, type); 
                     }
                     return s;
                   });
    int total_size = 0;
    std::string enumName = name + "_enum";
    std::string structList =
      listToString(parm, t->enumerations, ", ",
                   [&](ast::EnumerationElement& e){
                     total_size++;
                     std::string s;
                     if (e.identifier) {
                       std::string a = e.identifier->toString(true);
                       std::string b = e.identifier->toString();
                       s =  enumName + "::" + a + ", 0, \"" + b + "\"";
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
    std::string s = std::to_string(total_size);
    parm.println("enum class " + enumName + " {" + enumList + "};");
    parm.println("struct " + valueName + " {");
    parm.incIndent();
    parm.println("const static int size = " + s + ";");
    parm.println("const static int enum_size = " + std::to_string(enum_size) + ";");
    parm.println("EnumerationElement<" + enumName + "> array[size] {" + structList + "};");
    parm.decIndent();
    parm.println("};");
    parm.println("template<typename T=" + enumName+ ", class E=" + valueName + ">");
    parm.println("using " + name + " = Enumeration<T, E>;");
    return ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION, name);
  }

  void SystemC::printArrayType(parameters& parm, std::string& name, ast::ArrayDefinition* r, std::string& subtype) {
    debug.functionStart("printArrayType");
    assert(r);
    if (r->range) {
      std::string left, right;
      ast::ObjectValueContainer type(ast::ObjectValue::INTEGER);
      rangeToString(r->range, left, right, type);
      parm.println("vhdl_array_type(" + name + ", " + left + ", " + right + ", " + subtype + ");");
    } else if (r->subtype) {
      std::string id = r->subtype->identifier->toString(true);
      parm.println("vhdl_array_type(" + name + ", " + id + "<>::LEFT(), " + id + "<>::RIGHT(), " + subtype + ");");
    }
    debug.functionEnd("printArrayType");
  }
  
  std::string SystemC::ArraySubtype(parameters& parm, DatabaseResult& database_result,
                                    std::string& name, ast::SubtypeIndication* t) {
    debug.functionStart("ArraySubtype");
    assert(t);
    std::string typeName = t->name->toString(true);
    if (a_database.findOne(database_result, typeName, ast::ObjectType::TYPE)) { 
      typeName = a_database.namePrefix(database_result) + typeName;
      if (t->range) {
        printSubtype(parm, name, t->range, typeName, database_result.object->type);
        typeName = name;
      }
      typeName += "<>";
    } else {
      exceptions.printError("Could not find type \"" + typeName + "\"", &t->name->text);
    }
    debug.functionEnd("ArraySubtype");
    return typeName;
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
    debug.functionStart("arrayType");
    assert(t); 
    std::string name = identifier->toString(true);
    DatabaseResult database_result;
    std::string subtypeName = ArraySubtype(parm, database_result, name, t->type);
    printArrayType(parm, name, t->definition, subtypeName);
    ast::ObjectValueContainer value(ast::ObjectValue::ARRAY);
    value.setSubtype(database_result.object->type);
    debug.functionEnd("arrayType");
    return value;
  }

  ast::ObjectValueContainer SystemC::SimpleType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type,
                                                ast::ObjectValue object_value, std::string definition) {
    debug.functionStart("SimpleType");
    std::string name = identifier->toString(true);
    std::string type_name = type->toString(true);
    DatabaseResult database_result;
    ast::ObjectValueContainer value(object_value);
    if (a_database.findOne(database_result, type_name, ast::ObjectType::TYPE)) {
      value.setSubtype(database_result.object->type);
      std::string n = a_name_converter.getName(database_result, true);
      parm.println("template<class T = " + n + ">");
      parm.println("using " + name + " = " + definition + "<T>;"); 
    } else {
      exceptions.printError("Could not find type " + type_name, &type->text);
    }
    debug.functionEnd("SimpleType");
    return value;
  }

  ast::ObjectValueContainer SystemC::AccessType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type) {
    debug.functionStart("AccessType");
    ast::ObjectValueContainer value = SimpleType(parm, identifier, type, ast::ObjectValue::ACCESS, "sc_access");
    debug.functionEnd("AccessType");
    return value;
  }

  ast::ObjectValueContainer SystemC::FileType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type) {
    debug.functionStart("FileType");
    ast::ObjectValueContainer value = SimpleType(parm, identifier, type, ast::ObjectValue::FILE, "sc_file");
    debug.functionEnd("FileType");
    return value;
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      debug.functionStart("type_declarations");
      printSourceLine(parm, t->identifier);
      ast::ObjectValueContainer value;
      if (t->accessType) {
        value = AccessType(parm, t->identifier, t->accessType);
      } else if (t->fileType) {
        value = FileType(parm, t->identifier, t->fileType);
      } else {
        assert(t->typeDefinition);
        if (t->typeDefinition->numberType) {
          value = numberType(parm, t->identifier, t->typeDefinition->numberType);
        } else if (t->typeDefinition->enumerationType) {
          value = enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
        } else if (t->typeDefinition->arrayType) {
          value = arrayType(parm, t->identifier, t->typeDefinition->arrayType);
        } else {
          assert(false);
        }
      }
      std::string name = t->identifier->toString(true);
      a_database.add(ast::ObjectType::TYPE, name, value);
      debug.functionEnd("type_declarations");
    }
  }
  
  bool SystemC::findType(ast::SimpleIdentifier* identifier, ast::ObjectValueContainer& type) {
    assert(identifier);
    std::string name = identifier->toString(true);
    DatabaseResult object;
    if (a_database.findOne(object, name, ast::ObjectType::TYPE)) {
      type = object.object->type;
      return true;
    }
    exceptions.printError("Could not find declaration of type \"" + name + "\"", &identifier->text);
    return false;
  }

  void SystemC::generateObjectArguments(ast::InterfaceList* interface, ast::ObjectArguments& arguments) {
    if (interface) {
      for (ast::InterfaceElement& i : interface->interfaceElements.list) {
        ast::ObjectArgument a;
        a.name = i.object->identifier->toString(true);
        findType(i.object->type->name, a.type);
        a.defaultValue = (i.object->initialization) ? a_expression.toString(i.object->initialization->value, a.type) : "";
        arguments.push_back(a);
      }
    }
  }
  
  void SystemC::generateObjectArguments(ast::List<ast::SimpleIdentifier>* args, ast::ObjectArguments& arguments) {
    if (args) {
      for (ast::SimpleIdentifier& i : args->list) {
        ast::ObjectArgument a;
        a.name = "";
        findType(&i, a.type);
        a.defaultValue = "";
        arguments.push_back(a);
      }
    }
  }

  std::string SystemC::getArgumentTypes(parameters& parm, ast::InterfaceList* interface) {
    if (interface) {
      return listToString(parm, &interface->interfaceElements, ", ",
                          [](ast::InterfaceElement& e) {
                            return e.object->type->name->toString(true);
                          });
    }
    return "";
  }

  std::string SystemC::getArgumentNames(parameters& parm, ast::InterfaceList* interface) {
   if (interface) {
      return listToString(parm, &interface->interfaceElements, ", ",
                          [](ast::InterfaceElement& e) {
                            return e.object->identifier->toString(true);
                          });
    }
    return "";
  };
    
  std::string SystemC::getArgumentNames(parameters& parm, ast::AssociationList* arguments) {
    if (arguments) {
      return listToString(parm, &arguments->associationElements, ", ",
                          [](ast::AssociationElement& e) {
                            return e.formalPart->toString(true);
                          });
    }
    return "";
  }

  std::string SystemC::getArgumentTypes(parameters& parm, ast::List<ast::SimpleIdentifier>* arguments) {
    std::string result;
    if (arguments) {
      result = listToString(parm, arguments, ", ", 
                            [](ast::SimpleIdentifier& s) {
                              return s.toString(true);
                            });
    }
    return result;
  }

  std::string SystemC::getInterface(parameters& parm, ast::InterfaceList* interface) {
    std::string result;
    if (interface) {
      parameters::Area area = parm.area;
      parm.setArea(parameters::Area::INTERFACE);
      result = interfaceListToString(parm, interface, ", ", true);
      parm.setArea(area);
    }
    return result;
  }
  
  std::string SystemC::function_attribute(parameters& parm,
                                          std::string &name,
                                          ast::ObjectType type,
                                          std::string& interface,
                                          ast::ObjectArguments& arguments,
					  std::string returnType,
                                          ast::Text* text) {
    std::string foreignName = "";
    auto valid = [&](DatabaseElement* e) {
      return (e->id == type) && (e->arguments.equals(arguments));
    };
    DatabaseResult object;
    if (a_database.findOne(object, name, valid)) {
      DatabaseElement* e = object.object;
      if (e->attribute && e->attribute->expression) {
        parm.println("/*");
        parm.println(" * This is the definition of the foreign function set as an attribute.");
        parm.println(" * The implementation must be defined in a .cpp file in this directory.");
        parm.println("*/");
        foreignName = e->attribute->expression->toString(true);
        parm.println(returnType + " " + foreignName + interface + ";");
      }
    } else {
      exceptions.printError("Did not find declaration of " + ast::toString(type) + " \"" + name + "\"", text); 
      a_database.printAllObjects(name);
    }
    return foreignName;
  };
  
  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f) {
    if (f) {
      debug.functionStart("function_declarations");
      bool operatorName = (f->name == NULL);
      ast::Text& text = operatorName ? f->string->text : f->name->text;
      std::string name;
      std::string translatedName;
      if (operatorName) {
        name = f->string->toString(true);
        a_expression.translateOperator(name, translatedName);
        translatedName = "operator " + translatedName;
      } else {
        name = f->name->toString(true);
        translatedName = name;
      }
      ast::ObjectArguments arguments(true);
      generateObjectArguments(f->interface, arguments);
      ast::ObjectValueContainer returnType;
      findType(f->returnType, returnType);
      parm.returnType = returnType;
      printSourceLine(parm, text);
      std::string returnTypeName = f->returnType->toString(true);
      a_database.globalName(returnTypeName, ast::ObjectType::TYPE);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      a_database.addFunction(name, arguments, returnType, f);
      if (f->body) {
        std::string foreignFunctionName = function_attribute(parm, name, ast::ObjectType::FUNCTION,
                                                             interface, arguments, returnTypeName,
							     &text);
        std::string parentName = a_database.getParentName();
        //        descendHierarchy(parm, name);
        bool implementation = parm.isArea(parameters::Area::IMPLEMENTATION);
        std::string s = (implementation && !operatorName) ? parentName + "::" : "";
        parm.println(returnTypeName + " " + s + translatedName + interface + "{");
        parm.incIndent();
        if (!foreignFunctionName.empty()) {
          parm.println("// Foreign function call");
          parm.println("return " + foreignFunctionName + "(" + argumentNames + ");");
        }
        FunctionBody(parm, f->body->declarations, f->body->sequentialStatements);
        parm.decIndent();
        parm.println("}");
        // ascendHierarchy(parm);
      } else {
        parm.println((operatorName ? "friend " : "") + returnTypeName + " " + translatedName + interface + ";");
      }
      debug.functionEnd("function_declarations");
    }
  }

  void SystemC::procedure_declarations(parameters& parm, ast::ProcedureDeclaration* f) {
    if (f) {
      debug.functionStart("procedure_declarations");
      printSourceLine(parm, f->name->text);
      std::string name = f->name->toString(true);
      ast::ObjectArguments arguments(true);
      generateObjectArguments(f->interface, arguments);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      if (f->body) {
        std::string parentName = a_database.getParentName();
        std::string foreignFunctionName = function_attribute(parm, name, ast::ObjectType::PROCEDURE,
                                                             interface, arguments, "void", &f->name->text);
        bool implementation = parm.isArea(parameters::Area::IMPLEMENTATION);
        std::string s = implementation ? parentName + "::" : "";
        parm.println("void " + s + name + interface + "{");
        parm.incIndent();
        descendHierarchy(parm, name);
        if (!foreignFunctionName.empty()) {
          parm.println("// Foreign function call");
          parm.println(foreignFunctionName + "(" + argumentNames + ");");
        }
        FunctionBody(parm, f->body->declarations, f->body->sequentialStatements);
        ascendHierarchy(parm);
        parm.decIndent();
        parm.println("}");
      } else {
        a_database.addProcedure(name, arguments, f);
        parm.println("void " + name + interface + ";");
      }
      debug.functionEnd("procedure_declarations");
    }
    }

  void SystemC::FunctionBody(parameters& parm, ast::List<ast::Declaration>& d,
                             ast::List<ast::SequentialStatement>& s) {
    debug.functionStart("function_body");
    parameters::Area area = parm.area;
    parm.area = parameters::Area::IMPLEMENTATION;
    declarations(parm, d);
    sequentialStatements(parm, s);
    parm.area = area;
    debug.functionEnd("function_body");
  }
  
  void SystemC::attribute_declarations(parameters& parm, ast::Attribute* a) {
    if (a) {
      debug.functionStart("attribute_declarations");
      if (a->item || a->string) {
	ast::Text* text = a->item ? &a->item->text : &a->string->text;
	std::string name = a->item ? a->item->toString(true) : a->string->toString(true);
	ast::ObjectArguments arguments(false);
	generateObjectArguments(a->arguments, arguments);
	ast::ObjectType id = a->objectType;
	a_database.addAttribute(name, arguments, id, a, text);
      }
      debug.functionEnd("attribute_declarations");
    }
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
  
  void SystemC::printSubtype(parameters& parm, std::string& name, ast::RangeType* r,
                             std::string typeName, ast::ObjectValueContainer& type) {
    debug.functionStart("printSubtype");
    std::string rangeName;
    std::string left, right;
    rangeToString(r, left, right, type);
    rangeName = name + "_range";
    parm.println("struct " + rangeName + " {");
    parm.incIndent();
    parm.println(typeName + "_type left = " + left + ";");
    parm.println(typeName + "_type right = " + right + ";");
    parm.decIndent();
    parm.println("};");
    parm.println("template<class RANGE = " + rangeName + ">");
    parm.println("using " + name + " = " + typeName + "<RANGE>;");
    debug.functionEnd("printSubtype");
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
  std::string SystemC::subtypeIndication(parameters& parm, DatabaseResult& database_result,
                                         std::string& name, ast::SubtypeIndication* t) {
    debug.functionStart("subtypeIndication");
    assert(t);
    std::string typeName = t->name->toString(true);
    debug.debug("Search for " + typeName);
    if (a_database.findOne(database_result, typeName, ast::ObjectType::TYPE)) { 
      typeName = a_database.namePrefix(database_result) + typeName;
      debug.debug("Found typeName = " + typeName);
      if (t->range) {
        printSubtype(parm, name, t->range, typeName, database_result.object->type);
      } else if (!parm.isArea(parameters::Area::INTERFACE)) {
        parm.println("template <class RANGE = " + typeName + "_range>");
        parm.println("using " + name + " = " + typeName + "<>;");
      } 
      typeName += "<>";
    } else {
      exceptions.printError("Could not find type \"" + typeName + "\"", &t->name->text);
    }
    debug.debug("typeName = " + typeName);
    debug.functionEnd("subtypeIndication");
    return typeName;
  }

  /*
  vhdl:
    subtype type_t is integer range 0 to 10;
    subtype type_t is natural;
  systemc:
    struct TYPE_T_range { int left = 0; int right = 4; };
    template<class T = TYPE_T_range>
    using TYPE_T = INTEGER<T>;
  */
  void SystemC::subtype_declarations(parameters& parm, ast::SubtypeDeclaration* t) {
    if (t) {
      printSourceLine(parm, t->identifier);
      std::string name = t->identifier->toString(true);
      DatabaseResult database_result;
      subtypeIndication(parm, database_result, name, t->type);
      a_database.add(ast::ObjectType::TYPE, name, database_result.object->type);
    }
  }

  std::string SystemC::objectDeclarationToString(parameters& parm, ast::ObjectDeclaration* v,
                                                 bool initialization) {
    std::string s = "";
    if (v) {
      debug.functionStart("objectDeclarationToString");
      auto func = [&](std::string& name,
		      std::string& type, std::string& init,
		      ast::ObjectType id, ast::ObjectDeclaration::Direction direction) {
        if (id == ast::ObjectType::SIGNAL) {
          type = "sc_signal<" + type + ">";
        } 
        s = type + " " + name;
        if (initialization && init.size() > 0) {
          s += " = " + init;
        }
      };
      objectDeclaration(parm, v, func);
      debug.functionEnd("objectDeclarationToString");
    }
    return s;
  }

  void SystemC::object_declarations(parameters& parm, ast::ObjectDeclaration* v) {
    if (v) {
      debug.functionStart("object_declarations");
      printSourceLine(parm, v->identifier->text);
      std::string s = objectDeclarationToString(parm, v, true);
      parm.println(s + ";");
      debug.functionEnd("object_declarations");
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    debug.functionStart("declarations");
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      subtype_declarations(parm, i.subtype);
      object_declarations(parm, i.variable);
      object_declarations(parm, i.signal);
      object_declarations(parm, i.constant);
      function_declarations(parm, i.function);
      procedure_declarations(parm, i.procedure);
      attribute_declarations(parm, i.attribute);
    }
    debug.functionEnd("declarations");
  }

}

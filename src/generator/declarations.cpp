#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"
#include "database/database.hpp"
#include "definition.hpp"
#include "sequential.hpp"

namespace generator {

  /*
    vhdl:
    type test_t is range 1 to 20;
  */
  ast::ObjectValueContainer SystemC::numberType(parameters& parm, ast::SimpleIdentifier* identifier,
						ast::NumberType* t) {
    std::string name = identifier->toString(true);
    debug.functionStart("numberType(name = " + name + ")");
    assert(t);
    ast::ObjectValue value;
    if (t->physical) {
      printPhysicalType(parm, name, t);
      value = ast::ObjectValue::PHYSICAL;
    } else {
      printRangeType(parm, name, t->range);
      value = ast::ObjectValue::INTEGER;
    }
    ast::ObjectValueContainer type(value, name);
    debug.functionEnd("numberType: " + type.toString());
    return type;
  }
  
  ast::ObjectValueContainer SystemC::enumerationType(parameters& parm, ast::SimpleIdentifier* identifier, ast::EnumerationType* t) {
    debug.functionStart("enumerationType");
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
                       parm.addObject(ast::ObjectType::ENUM, s, type);
                     } else if (e.character) {
                       std::string name = e.character->toString();
                       parm.addObject(ast::ObjectType::ENUM, name, type); 
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
    parm.addClassContents("enum class " + enumName + " {" + enumList + "};", __FILE__, __LINE__);
    parm.addClassContents("struct " + valueName + " {", __FILE__, __LINE__);
    parm.addClassContents("const static int size = " + s + ";", __FILE__, __LINE__);
    parm.addClassContents("const static int enum_size = " + std::to_string(enum_size) + ";", __FILE__, __LINE__);
    parm.addClassContents("vhdl::EnumerationElement<" + enumName + "> array[size] {" + structList + "};", __FILE__, __LINE__);
    parm.addClassContents("};", __FILE__, __LINE__);
    parm.addClassContents("using " + name + " = vhdl::Enumeration<" + enumName + ", " + valueName + ">;", __FILE__, __LINE__);
    auto f = [&](parameters& parm, std::string& left, std::string& right, std::string& ascending) {
    };
    PrintFactory(parm, name, f);
    debug.functionEnd("enumerationType");
    return ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION, name);
  }

  void SystemC::PrintFactory(parameters& parm, const std::string& name, 
                             ast::RangeType* range, ast::SimpleIdentifier* identifier,
                             ast::ObjectValue expected_value,
                             ast::ArraySubtypeDefinition* subtype) {
    debug.functionStart("PrintFactory(name = " + name + ", expected_value = " + ast::toString(expected_value) + ")");
    auto f = [&](parameters& parm, std::string& left, std::string& right, std::string& ascending) {
      if (range) {
        ast::ObjectValueContainer type(expected_value);
        rangeToString(parm, range, left, right, ascending, type);
      } else if (identifier || subtype) {
        ast::SimpleIdentifier* id = (identifier ? identifier : subtype->identifier);
	assert(id);
	std::string type_name = id->toString(true);
        DatabaseResult database_result;
        if (parm.findOne(database_result, type_name, ast::ObjectType::TYPE)) { 
          std::string id = NameConverter::getName(parm, database_result, true);
          left = id + ".LEFT()";
          right = id + ".RIGHT()";
        } else {
          exceptions.printError("Could not find type " + type_name, &id->text);
	  if (a_verbose) {parm.printDatabase();}
	}
      }
    };
    PrintFactory(parm, name, f);
    debug.functionEnd("PrintFactory");
  }
  
  void SystemC::printArrayType(parameters& parm, std::string& name, ast::List<ast::ArrayDefinition>& definition,
                               std::string& subtype, ast::ObjectValueContainer::Array& arguments) {
    debug.functionStart("printArrayType");
    std::string array_template_start;
    std::string array_template_end;
    for (auto& r : definition.list) { 
      ast::ObjectValue type;
      std::string range;
      if (r.range) {
        if (r.range->range_direction_type) {
          ast::ObjectValueContainer t;
          a_expression.collectUniqueReturnType(parm, r.range->range_direction_type->left, t);
          type = t.GetValue();
          range = "vhdl::Range<int>";
          arguments.push_back(t);
        } else {
          assert(false);
        }
      } else if (r.identifier) {
        DatabaseResult database_result;
        type = ast::ObjectValue::ENUMERATION;
        auto f = [&](DatabaseElement* e) {
          return e->type.IsValue(type);
        };
        if (parm.findOne(database_result, r.identifier, f)) { 
          range = NameConverter::getName(parm, database_result);
          arguments.push_back(database_result.object->type);
        }
      } else if (r.subtype) {
        DatabaseResult database_result;
        if (parm.findOne(database_result, r.subtype->identifier)) { 
          range = NameConverter::getName(parm, database_result);
          arguments.push_back(database_result.object->type);
          type = database_result.object->type.GetValue();
        }
      }
      array_template_start = "vhdl::Array<" + range + ", " + array_template_start;
      array_template_end += ">";
    }
    parm.addClassContents("using " + name + " = " + array_template_start + subtype + array_template_end + ";", __FILE__, __LINE__);
    auto f = [&](parameters& parm, std::string& left, std::string& right, std::string& ascending) {
      for (auto& r : definition.list) { 
        if (r.range) {
          if (r.range->range_direction_type) {
            ast::ObjectValueContainer t;
            a_expression.collectUniqueReturnType(parm, r.range->range_direction_type->left, t);
            rangeToString(parm, r.range, left, right, ascending, t);
          } else {
            assert(false);
          }
        }
      }
    };
    PrintFactory(parm, name, f);
    debug.functionEnd("printArrayType");
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
    std::string type_name = t->type->name->toString(true);
    DatabaseResult database_result;
    ast::ObjectValueContainer value;
    if (parm.findOne(database_result, type_name, ast::ObjectType::TYPE)) { 
      std::string subtypeName = NameConverter::getName(parm, database_result);
      ast::ObjectValueContainer::Array arguments;
      printArrayType(parm, name, t->definition, subtypeName, arguments);
      value = ast::ObjectValueContainer(ast::ObjectValue::ARRAY, arguments, database_result.object->type);
    } else {
      exceptions.printError("Could not find type \"" + type_name + "\"", &identifier->text);
      if (a_verbose) {parm.printDatabase();}
    }
    debug.functionEnd("arrayType");
    return value;
  }

  ast::ObjectValueContainer SystemC::SimpleType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type,
                                                ast::ObjectValue object_value, std::string definition) {
    debug.functionStart("SimpleType");
    std::string name = identifier->toString(true);
    std::string type_name = type->toString(true);
    DatabaseResult database_result;
    ast::ObjectValueContainer value;
    if (parm.findOne(database_result, type_name, ast::ObjectType::TYPE)) {
      value = ast::ObjectValueContainer(object_value, database_result.object->type);
      std::string n = NameConverter::getName(parm, database_result);
      parm.addClassContents("using " + name + " = " + definition + "<" + n + ">;", __FILE__, __LINE__); 
      auto f = [&](parameters& parm, std::string& left, std::string& right, std::string& ascending) {
      };
      PrintFactory(parm, name, f);
    } else {
      exceptions.printError("Could not find type " + type_name, &type->text);
      if (a_verbose) {parm.printDatabase();}
    }
    debug.functionEnd("SimpleType");
    return value;
  }

  ast::ObjectValueContainer SystemC::AccessType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type) {
    debug.functionStart("AccessType");
    ast::ObjectValueContainer value = SimpleType(parm, identifier, type, ast::ObjectValue::ACCESS, "vhdl::vhdl_access");
    debug.functionEnd("AccessType");
    return value;
  }

  ast::ObjectValueContainer SystemC::FileType(parameters& parm, ast::SimpleIdentifier* identifier, ast::SimpleIdentifier* type) {
    debug.functionStart("FileType");
    ast::ObjectValueContainer value = SimpleType(parm, identifier, type, ast::ObjectValue::FILE, "vhdl::vhdl_file");
    debug.functionEnd("FileType");
    return value;
  }

  void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    std::string name = t->identifier->toString(true);
    debug.functionStart("type_declarations(name = " + name + ")");
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
    debug.debug("Hierarchy: " + parm.file_container.getClassContainerHierarchy());
    parm.addObject(ast::ObjectType::TYPE, name, value);
    debug.functionEnd("type_declarations");
  }

  void SystemC::FileDeclaration(parameters& parm, ast::FileDeclaration* file) {
    if (file) {
      std::string name = file->handle->toString(true);
      std::string type = file->type->toString(true);
      std::string direction = file->direction->toString(true);
      debug.functionStart("FileDeclaration(name = " + name + ", type = " + type + ", direction = " + direction + ")");
      DatabaseResult result;
      if (parm.findOne(result, type, ast::ObjectType::TYPE)) {
        type = NameConverter::getName(parm, result);
        parm.addObject(ast::ObjectType::FILE, name, result.object->type);
      }
      if (parm.findOne(result, direction)) {
        direction = NameConverter::getName(parm, result);
      }
      parm.addClassContents(getSourceLine(file->handle), __FILE__, __LINE__);
      parm.addClassContents(type + " " + name + " = " + type + "(" +
			    direction + ", " + file->filename->toString() + ");", __FILE__, __LINE__); 
      debug.functionEnd("FileDeclaration");
    }
  }
  
  void SystemC::AliasDeclaration(parameters& parm, ast::AliasDeclaration* alias) {
    if (alias) {
      debug.functionStart("AliasDeclaration");
      std::string designator = alias->designator->toString(true);
      std::string name = alias->name->toString(true);
      std::string type;
      DatabaseResult result;
      if (parm.findOne(result, alias->name)) {
        type = NameConverter::getName(parm, result);
        parm.addObject(result.object->id, designator, result.object->type);
      }
      parm.addClassContents(getSourceLine(alias->designator), __FILE__, __LINE__);
      parm.addClassContents("using " + designator + " = " + type + ";", __FILE__, __LINE__); 
      debug.functionEnd("AliasDeclaration");
    }
  }
  
  void SystemC::generateObjectArguments(parameters& parm, ast::InterfaceList* interface, ast::ObjectArguments& arguments) {
    if (interface) {
      for (ast::InterfaceElement& i : interface->interfaceElements.list) {
        ast::ObjectArgument a;
        DatabaseResult result;
        a.type_name = i.object->type->name->toString(true);
        if (parm.findOne(result, a.type_name, ast::ObjectType::TYPE)) {
	  a.type = result.object->type;
	  a.default_value = (i.object->initialization) ? i.object->initialization->value : NULL;
	  for (ast::SimpleIdentifier& id : i.object->identifiers.list) { 
	    a.name = id.toString(true);
	    arguments.push_back(a);
	  }
	}
      }
    }
  }
  
  void SystemC::generateObjectArguments(parameters& parm, ast::List<ast::SimpleIdentifier>* args, ast::ObjectArguments& arguments) {
    if (args) {
      for (ast::SimpleIdentifier& i : args->list) {
        ast::ObjectArgument a;
        a.name = "";
        DatabaseResult result;
        a.type_name = i.toString(true);
        if (parm.findOne(result, a.type_name, ast::ObjectType::TYPE)) {
	  a.type = result.object->type;
	  a.default_value = NULL;
	  arguments.push_back(a);
	} else {
	  debug.debug("Did not find object argument type " + a.type_name);
	}
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
     auto v = [&](ast::InterfaceElement& e) {
       auto f = [&](ast::SimpleIdentifier& i) {
         return i.toString(true);
       };
       return listToString(parm, e.object->identifiers.list, ", ", f);
     };
     return listToString(parm, &interface->interfaceElements, ", ", v);
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

  std::string SystemC::InterfaceTypeConverter(std::string& type, ast::ObjectType id,
                                              ast::ObjectDeclaration::Direction direction) {
    std::string r = (direction == ast::ObjectDeclaration::Direction::OUT ||
                     direction == ast::ObjectDeclaration::Direction::INOUT ||
                     id == ast::ObjectType::FILE) ? "&" : ""; 
    return type + r;
  }
  
  std::string SystemC::GetInterface(parameters& parm, ast::InterfaceList* interface, bool initialization,
                                    std::string local_prefix) {
    debug.functionStart("GetInterface");
    std::string result;
    if (interface) {
      bool quiet = parm.setQuiet(true);
      auto type_converter = [&](std::string& type, ast::ObjectType id,
                                ast::ObjectDeclaration::Direction direction) {
        return InterfaceTypeConverter(type, id, direction);
      };
      result = interfaceListToString(parm, interface, ", ", initialization, type_converter, local_prefix, false);
      parm.setQuiet(quiet);
    }
    debug.functionEnd("GetInterface");
    return result;
  }

  void SystemC::StoreInterfaceInDatabase(parameters& parm, ast::InterfaceList* interface) {
    debug.functionStart("StoreInterfaceInDatabase");
    if (interface) {
      bool quiet = parm.setQuiet(true);
      interfaceListToString(parm, interface, ", ", false, "", true);
      parm.setQuiet(quiet);
    }
    debug.functionEnd("StoreInterfaceInDatabase");
  }

  void SystemC::PrintInterface(parameters& parm, ast::InterfaceList* interface) {
    debug.functionStart("PrintInterface");
    std::string result;
    if (interface) {
      auto type_converter = [&](std::string& type, ast::ObjectType id,
                                ast::ObjectDeclaration::Direction direction) {
        return InterfaceTypeConverter(type, id, direction);
      };
      result = interfaceListToString(parm, interface, ", ", true, type_converter, "", true);
    }
    debug.functionEnd("PrintInterface");
  }

  std::string SystemC::AttributeName(ast::Attribute* a) {
    return a->expression->toString(true, true);
  }
  
  std::string SystemC::FunctionAttribute(parameters& parm,
                                         std::string &name,
                                         ast::ObjectType type,
                                         ast::ObjectArguments& arguments,
                                         ast::Text* text) {
    debug.functionStart("FunctionAttribute, name = " + name + ", arguments = " + arguments.toString());
    std::string foreignName = "";
    auto valid = [&](DatabaseElement* e) {
      bool match = (e->id == type) && e->attribute && e->arguments.ExactMatch(arguments);
      debug.debug((match ? "Match = " : "Different = ") + e->toString(), true,
                  match ? Output::Color::GREEN : Output::Color::RED);
      return match;
    };
    DatabaseResult object;
    if (parm.findOneBase(object, name, valid)) {
      DatabaseElement* e = object.object;
      assert(e != NULL);
      if (e->attribute && e->attribute->expression) {
        foreignName = AttributeName(e->attribute);
      }
    } else {
      debug.debug("Did not find attribute of function " + name);
    }
    debug.debug("Foreign name = " + foreignName);
    debug.functionEnd("FunctionAttribute");
    return foreignName;
  };

  std::string SystemC::FunctionReturn(parameters& parm, ast::FunctionDeclaration* f, bool global_scope) {
    debug.functionStart("FunctionReturn");
    std::string returnTypeName = "void";
    parm.returnType = ast::ObjectValueContainer(ast::ObjectValue::NONE);
    if (f->returnType) {
      DatabaseResult returnType;
      if (parm.findOne(returnType, f->returnType, ast::ObjectType::TYPE)) {
	returnTypeName = NameConverter::getName(parm, returnType, false, "", global_scope);
        parm.returnType = returnType.object->type;
      }
    }
    debug.functionEnd("FunctionReturn: name = " + returnTypeName);
    return returnTypeName;
  }
  
  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* function_declaration) {
    if (function_declaration) {
      bool operatorName = (function_declaration->name == NULL);
      std::string origin_name = operatorName ? function_declaration->string->toString(true) :
	function_declaration->name->toString(true);
      debug.functionStart("function_declarations(name = " + origin_name + ")");
      parm.package_contains_function = true;
      ast::Text& text = operatorName ? function_declaration->string->text : function_declaration->name->text;
      
      ast::ObjectType type = function_declaration->type;
      std::string translatedName = origin_name;
      std::string class_name = origin_name;
      if (operatorName) {
        translatedName = "operator " + a_expression.translateOperator(parm, origin_name);
        class_name = "line" + std::to_string(text.getLine());
      } 
      ast::ObjectArguments arguments(true);
      generateObjectArguments(parm, function_declaration->interface, arguments);
      std::string localReturnTypeName = FunctionReturn(parm, function_declaration, false);
      std::string globalReturnTypeName = FunctionReturn(parm, function_declaration, true);
      parm.addImplementationContents(getSourceLine(text), __FILE__, __LINE__);
      std::string argumentNames = getArgumentNames(parm, function_declaration->interface);
      parm.addFunction(type, origin_name, arguments, parm.returnType, function_declaration, &text);
      if (!parm.parse_declarations_only) {
        class_name = NameConverter::getName(origin_name, arguments, parm.returnType);
	ParentInfo parent_info;
        parm.getParent(parent_info);
        bool package_body = (parent_info.type == ast::ObjectType::PACKAGE_BODY);
        std::string interface_with_initialization = "(" + GetInterface(parm, function_declaration->interface, true) + ")";
        std::string interface_without_initialization = "(" + GetInterface(parm, function_declaration->interface, false) + ")";
        if (function_declaration->body) {
          auto callback = [&](parameters& parm) {
            PrintInterface(parm, function_declaration->interface);
            if (function_declaration->body) {
              std::string foreignFunctionName = FunctionAttribute(parm, origin_name, type, arguments, &text);
              std::string interface = package_body ? interface_without_initialization : interface_with_initialization;
              parm.addClassContents(localReturnTypeName + " " + "run" + interface + ";", __FILE__, __LINE__);
	      std::string global_prefix = parm.hierarchyToString("::", true) + "::";
	      parm.addImplementationContents(globalReturnTypeName + " " + global_prefix + "run" + interface_without_initialization + "{", __FILE__, __LINE__);
              if (!foreignFunctionName.empty()) {
                parm.addImplementationContents("// Foreign function call", __FILE__, __LINE__);
                parm.addImplementationContents("return p->" + foreignFunctionName + "(" + argumentNames + ");", __FILE__, __LINE__);
              }
	      std::list<std::string> sequential_list;
	      auto sensitivity_list_callback = 
		[&](DatabaseResult& object, std::string& name_extension) {};
	      sequentialStatements(parm, function_declaration->body->sequentialStatements, sequential_list, 
				   sensitivity_list_callback);
	      parm.addImplementationContents(sequential_list);
              parm.addImplementationContents("}", __FILE__, __LINE__);
            } else {
              parm.addClassContents(localReturnTypeName + " run" + interface_with_initialization + ";", __FILE__, __LINE__);
            }
          };
	  defineObject(parm, false, class_name, type, "", NULL, NULL,
                       &function_declaration->body->declarations, NULL, callback, false, true);
        } 
        std::string interface = "(" + GetInterface(parm, function_declaration->interface, !package_body, class_name + "::") + ")";
        if (function_declaration->body) {
	  debug.debug("Function declaration body");
	  parm.addClassContents((operatorName ? "friend " : "") +
				localReturnTypeName + " " +
				translatedName + interface + ";", __FILE__, __LINE__);
          std::string global_prefix = parm.hierarchyToString("::", true) + "::";
	  parm.addImplementationContents((operatorName ? "friend " : "") +
					 globalReturnTypeName + " " +
					 global_prefix + translatedName + interface_without_initialization + " {", __FILE__, __LINE__);
          parm.addImplementationContents("auto inst = " + NameConverter::objectName(type, class_name) + "(this);", __FILE__, __LINE__);
          std::string s,d;
          for (auto& i : arguments.list) {
            s += d + i.name;
            d = ", ";
          }
          std::string r = localReturnTypeName == "void" ? "" : "return ";
          parm.addImplementationContents(r + "inst.run(" + s + ");", __FILE__, __LINE__);
          parm.addImplementationContents("}", __FILE__, __LINE__);
        } else {
	  parm.addClassContents("virtual " + std::string(operatorName ? "friend " : "") + localReturnTypeName + " " +
				translatedName + interface + " = 0;", __FILE__, __LINE__);
	}
      }
      debug.functionEnd("function_declarations");
    }
  }

  void SystemC::ForeignAttribute(parameters& parm, ast::Attribute* a) {
    debug.functionStart("ForeignAttribute");
    ast::Text* text = a->item ? &a->item->text : &a->string->text;
    std::string name = a->item ? a->item->toString(true) : a->string->toString(true);
    ast::ObjectArguments arguments(false);
    generateObjectArguments(parm, a->arguments, arguments);
    ast::ObjectType id = a->objectType;
    debug.debug("id = " + ast::toString(id) + ", name = " + name + ", arguments = " + arguments.toString());
    parm.addAttribute(name, arguments, id, a, text);
    auto valid = [&](DatabaseElement* e) {
      bool arguments_match = arguments.empty() || e->arguments.ExactMatch(arguments);
      bool match = (e->id == id) && arguments_match;
      std::string s = (match ? "Match" : "Different"); 
      debug.debug(s + ": " + e->toString());
      return match;
    };
    DatabaseResult object;
    if (parm.findOneBase(object, name, valid)) {
      DatabaseElement* e = object.object;
      assert(e->function);
      ast::InterfaceList* i = e->function->interface;
      std::string interface = "(" + GetInterface(parm, i) + ")";
      PrintInterface(parm, i);
      std::string returnName = "void";
      if (id == ast::ObjectType::FUNCTION) {
        DatabaseResult result;
        if (parm.findOne(result, e->function->returnType, ast::ObjectType::TYPE)) {
	  returnName = NameConverter::getName(parm, result);
	} else {
	  debug.debug("Could not find function return type");
	}
      }
      parm.addClassContents("/*", __FILE__, __LINE__);
      parm.addClassContents(" * This is the definition of the foreign function set as an attribute.", __FILE__, __LINE__);
      parm.addClassContents(" * The implementation must be defined in a .cpp file in this directory.", __FILE__, __LINE__);
      parm.addClassContents("*/", __FILE__, __LINE__);
      std::string foreignName = AttributeName(a);
      parm.addClassContents(returnName + " " + foreignName + interface + ";", __FILE__, __LINE__);
    } else {
      exceptions.printError("Did not find declaration of " + ast::toString(id) + " \"" + name + "\"", text); 
      parm.printAllObjects(name);
    }
    debug.functionEnd("ForeignAttribute");
  }
  
  void SystemC::attribute_declarations(parameters& parm, ast::Attribute* a) {
    if (a) {
      debug.functionStart("attribute_declarations");
      if (a->item || a->string) {
        static std::unordered_map<std::string, bool> ignored_attributes =
          {{"SYNTHESIS_RETURN", false}};
        assert(a->identifier);
        std::string identifier = a->identifier->toString(true);
        if (identifier == "FOREIGN") {
          ForeignAttribute(parm, a);
        } else if (ignored_attributes.find(identifier) != ignored_attributes.end()) {
          debug.debug("Ignoring " + identifier + " attribute");
        } else {
          exceptions.printWarning("Unknown attribute " + identifier, &a->identifier->text);
        }
      }
      debug.functionEnd("attribute_declarations");
    }
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
    std::string name = t->identifier->toString(true);
    debug.functionStart("subtype_declarations(name = " + name + ")");
    std::string type_name = t->type->name->toString(true);
    DatabaseResult database_result;
    if (parm.findOne(database_result, type_name, ast::ObjectType::TYPE)) {
      std::string type_name = NameConverter::getName(parm, database_result, false);
      parm.addObject(ast::ObjectType::TYPE, name, database_result.object->type);
      parm.addClassContents(getSourceLine(t->identifier), __FILE__, __LINE__);
      parm.addClassContents("using " + name + " = " + type_name + ";", __FILE__, __LINE__);
      PrintFactory(parm, name, t->type->range, NULL, database_result.object->type.GetValue());
    } else {
      exceptions.printError("Could not find type \"" + type_name + "\"", &t->identifier->text);
      if (a_verbose) {parm.printDatabase();}
    }
    debug.functionEnd("subtype_declarations");
  }

  void SystemC::ObjectDeclarations(parameters& parm, ast::ObjectDeclaration* v) {
    if (v) {
      debug.functionStart("ObjectDeclarations");
      auto func = [&](std::string& name, std::string& type, std::string& init,
		      std::string& factory_name, ast::ObjectType id,
                      ast::ObjectDeclaration::Direction direction) {
        if (id == ast::ObjectType::SIGNAL) {
          type = "vhdl::interface<sc_signal<" + type + ">, " + type + ">";
        }
        std::string s = type + " " + name;
	parm.addClassContents(getSourceLine(v->text), __FILE__, __LINE__);
      	parm.addClassContents(s + ";", __FILE__, __LINE__);
        parm.addClassConstructorContents(getSourceLine(v->text), __FILE__, __LINE__);
      	parm.addClassConstructorContents(name + ".construct(" + factory_name + ");", __FILE__, __LINE__);
        if (init.size() > 0) {
          parm.addClassConstructorContents(name + " = " + init + ";", __FILE__, __LINE__);
        }
      };
      ObjectDeclaration(parm, v, func);
      debug.functionEnd("ObjectDeclarations");
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    debug.functionStart("declarations");
    for (ast::Declaration i : d.list) {
      if (i.type) {type_declarations(parm, i.type);}
      if (i.subtype) {subtype_declarations(parm, i.subtype);}
      ObjectDeclarations(parm, i.variable);
      ObjectDeclarations(parm, i.signal);
      ObjectDeclarations(parm, i.constant);
      function_declarations(parm, i.function);
      attribute_declarations(parm, i.attribute);
      FileDeclaration(parm, i.file);
      AliasDeclaration(parm, i.alias);
    }
    debug.functionEnd("declarations");
  }

}

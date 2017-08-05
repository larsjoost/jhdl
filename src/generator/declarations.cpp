#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"
#include "database/database.hpp"
#include "definition.hpp"

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
    auto func = [](ast::RangeType* r, std::string& name, std::string& type_name) {
      std::string s = type_name;
      if (r) {s = name;}
      return s;
    };
    std::string typeName =  Subtype(parm, database_result, name, t, func);
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
  
  void SystemC::generateObjectArguments(ast::InterfaceList* interface, ast::ObjectArguments& arguments) {
    if (interface) {
      for (ast::InterfaceElement& i : interface->interfaceElements.list) {
        ast::ObjectArgument a;
        a.name = i.object->identifier->toString(true);
        DatabaseResult result;
        a.type_name = i.object->type->name->toString(true);
        a_database.findOne(result, a.type_name, ast::ObjectType::TYPE);
        a.type = result.object->type;
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
        DatabaseResult result;
        a.type_name = i.toString(true);
        a_database.findOne(result, a.type_name, ast::ObjectType::TYPE);
        a.type = result.object->type;
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

  std::string SystemC::GetInterface(parameters& parm, ast::InterfaceList* interface, bool initialization,
                                    std::string local_prefix) {
    debug.functionStart("GetInterface");
    std::string result;
    if (interface) {
      bool quiet = parm.setQuiet(true);
      result = interfaceListToString(parm, interface, ", ", initialization, local_prefix);
      parm.setQuiet(quiet);
    }
    debug.functionEnd("GetInterface");
    return result;
  }

  void SystemC::PrintInterface(parameters& parm, ast::InterfaceList* interface) {
    debug.functionStart("PrintInterface");
    std::string result;
    if (interface) {
      parameters::Area area = parm.area;
      parm.setArea(parameters::Area::INTERFACE);
      result = interfaceListToString(parm, interface, ", ", true);
      parm.setArea(area);
    }
    debug.functionEnd("PrintInterface");
  }

  std::string SystemC::AttributeName(ast::Attribute* a) {
    return a->expression->toString(true);
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
    if (a_database.findOne(object, name, valid)) {
      DatabaseElement* e = object.object;
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

  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f) {
    if (f) {
      debug.functionStart("function_declarations");
      bool operatorName = (f->name == NULL);
      ast::Text& text = operatorName ? f->string->text : f->name->text;
      std::string origin_name = operatorName ? f->string->toString(true) : f->name->toString(true);
      ast::ObjectType type = f->type;
      std::string translatedName = origin_name;
      std::string class_name = origin_name;
      if (operatorName) {
        a_expression.translateOperator(origin_name, translatedName);
        translatedName = "operator " + translatedName;
        class_name = "line" + std::to_string(text.getLine());
      } 
      ast::ObjectArguments arguments(true);
      generateObjectArguments(f->interface, arguments);
      std::string returnTypeName = "void";
      parm.returnType = ast::ObjectValueContainer(ast::ObjectValue::NONE);
      if (f->returnType) {
        DatabaseResult returnType;
        if (a_database.findOne(returnType, f->returnType, ast::ObjectType::TYPE)) {
          returnTypeName = a_name_converter.getName(returnType, true);
          parm.returnType = returnType.object->type;
        }
      }
      printSourceLine(parm, text);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      a_database.addFunction(type, origin_name, arguments, parm.returnType, f);
      class_name = "function_" + a_name_converter.getName(origin_name, arguments, parm.returnType);
      std::string parentName = a_database.getParentName();
      std::string function_prefix = "function_";
      std::string prefix = parentName + "::" + class_name + "::";
      std::string run_prefix;
      std::string interface_with_initialization = "(" + GetInterface(parm, f->interface, true) + ")";
      std::string interface_without_initialization = "(" + GetInterface(parm, f->interface, false) + ")";
      bool implementation = parm.isArea(parameters::Area::IMPLEMENTATION);
      auto createDefinition = [&](parameters& parm) {
        PrintInterface(parm, f->interface);
      };
      auto createBody = [&](parameters& parm) {
        if (f->body) {
          std::string foreignFunctionName = FunctionAttribute(parm, origin_name, type, arguments, &text);
          std::string interface = implementation ? interface_without_initialization : interface_with_initialization;
          parm.println(returnTypeName + " " + run_prefix + "run" + interface + "{");
          parm.incIndent();
          if (!foreignFunctionName.empty()) {
            parm.println("// Foreign function call");
            parm.println("return p->" + foreignFunctionName + "(" + argumentNames + ");");
          } 
          FunctionBody(parm, f->body->declarations, f->body->sequentialStatements);
          parm.decIndent();
          parm.println("}");
        } else {
          parm.println(returnTypeName + " run" + interface_with_initialization + ";");
        }
      };
      if (implementation) {
        if (f->body) {
          run_prefix = prefix;
          descendHierarchy(parm, parentName);
          createBody(parm);
          ascendHierarchy(parm);
        }
      } else {
        defineObject(parm, false, class_name, "SC_FUNCTION", NULL,
                     NULL, NULL, createBody, createDefinition, false);
      }
      std::string interface = "(" + GetInterface(parm, f->interface, !implementation, class_name + "::") + ")";
      bool define_function = implementation || f->body;
      parm.println((operatorName ? "friend " : "") + returnTypeName + " " +
                   (implementation ? parentName + "::" : "") + translatedName + interface +
                   (define_function ? "{" : ";"));
      if (define_function) {
        parm.incIndent();
        parm.println("auto inst = " + class_name + "(this);");
        std::string s,d;
        for (auto& i : arguments.list) {
          s += d + i.name;
          d = ", ";
        }
        std::string r = returnTypeName == "void" ? "" : "return ";
        parm.println(r + "inst.run(" + s + ");");
        parm.decIndent();
        parm.println("}");
      }
      debug.functionEnd("function_declarations");
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
  
  void SystemC::ForeignAttribute(parameters& parm, ast::Attribute* a) {
    debug.functionStart("ForeignAttribute");
    ast::Text* text = a->item ? &a->item->text : &a->string->text;
    std::string name = a->item ? a->item->toString(true) : a->string->toString(true);
    ast::ObjectArguments arguments(false);
    generateObjectArguments(a->arguments, arguments);
    ast::ObjectType id = a->objectType;
    debug.debug("id = " + ast::toString(id) + ", name = " + name + ", arguments = " + arguments.toString());
    a_database.addAttribute(name, arguments, id, a, text);
    auto valid = [&](DatabaseElement* e) {
      bool arguments_match = arguments.empty() || e->arguments.ExactMatch(arguments);
      bool match = (e->id == id) && arguments_match;
      std::string s = (match ? "Match" : "Different"); 
      debug.debug(s + ": " + e->toString());
      return match;
    };
    DatabaseResult object;
    if (a_database.findOne(object, name, valid)) {
      DatabaseElement* e = object.object;
      assert(e->function);
      ast::InterfaceList* i = e->function->interface;
      std::string interface = "(" + GetInterface(parm, i) + ")";
      PrintInterface(parm, i);
      std::string returnName = "void";
      if (id == ast::ObjectType::FUNCTION) {
        DatabaseResult result;
        a_database.findOne(result, e->function->returnType, ast::ObjectType::TYPE);
        returnName = a_name_converter.getName(result, true);
      }
      parm.println("/*");
      parm.println(" * This is the definition of the foreign function set as an attribute.");
      parm.println(" * The implementation must be defined in a .cpp file in this directory.");
      parm.println("*/");
      std::string foreignName = AttributeName(a);
      parm.println(returnName + " " + foreignName + interface + ";");
    } else {
      exceptions.printError("Did not find declaration of " + ast::toString(id) + " \"" + name + "\"", text); 
      a_database.printAllObjects(name);
    }
    debug.functionEnd("ForeignAttribute");
  }
  
  void SystemC::attribute_declarations(parameters& parm, ast::Attribute* a) {
    if (a) {
      debug.functionStart("attribute_declarations");
      if (a->item || a->string) {
        assert(a->identifier);
        std::string identifier = a->identifier->toString(true);
        if (identifier == "FOREIGN") {
          ForeignAttribute(parm, a);
        } else {
          exceptions.printWarning("Unknown attribute " + identifier, &a->identifier->text);
        }
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
      auto func = [&](ast::RangeType* r, std::string& name, std::string& type_name) {
        if (!r && !parm.isArea(parameters::Area::INTERFACE)) {
          parm.println("template <class RANGE = " + type_name + "_range>");
          parm.println("using " + name + " = " + type_name + "<>;");
        } 
        return type_name;
      };
      Subtype(parm, database_result, name, t->type, func);
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
      attribute_declarations(parm, i.attribute);
    }
    debug.functionEnd("declarations");
  }

}

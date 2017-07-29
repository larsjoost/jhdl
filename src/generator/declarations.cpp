#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"
#include "database/database.hpp"

namespace generator {

   void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
     if (t) {
       debug.functionStart("type_declarations");
       assert(t->typeDefinition);
       std::string name = t->identifier->toString(true);
       ast::ObjectValueContainer value;
       if (t->typeDefinition->numberType) {
         value = numberType(parm, t->identifier, t->typeDefinition->numberType);
       } else if (t->typeDefinition->enumerationType) {
         value = enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
       } else if (t->typeDefinition->arrayType) {
         value = arrayType(parm, t->identifier, t->typeDefinition->arrayType);
       } else {
         assert(false);
       }
       database.add(ast::TYPE, name, value);
       debug.functionEnd("type_declarations");
     }
  }

  bool SystemC::findType(ast::SimpleIdentifier* identifier, ast::ObjectValueContainer& type) {
    assert(identifier);
    std::string name = identifier->toString(true);
    DatabaseResult object;
    if (database.findOne(object, name, ast::TYPE)) {
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
        ExpressionParser expr(&database);
        a.defaultValue = (i.object->initialization) ? expr.toString(i.object->initialization->value, a.type) : "";
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
    if (arguments) {
      return listToString(parm, arguments, ", ", 
                          [](ast::SimpleIdentifier& s) {
                            return s.toString(true);
                          });
    }
    return "";
  }

  std::string SystemC::getInterface(parameters& parm, ast::InterfaceList* interface) {
    if (interface) {
      return interfaceListToString(parm, interface, ", ", false);
    }
    return "";
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
    if (database.findOne(object, name, valid)) {
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
      database.printAllObjects(name);
    }
    return foreignName;
  };
  
  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f,
                                      bool implementation) {
    if (f) {
      debug.functionStart("function_declarations");
      bool operatorName = (f->name == NULL);
      ast::Text& text = operatorName ? f->string->text : f->name->text;
      std::string name;
      std::string translatedName;
      if (operatorName) {
        ExpressionParser expr(&database);
        name = f->string->toString(true);
        expr.translateOperator(name, translatedName);
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
      database.globalName(returnTypeName, ast::TYPE);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      if (f->body) {
        std::string parentName = database.getParentName();
        descendHierarchy(parm, name);
        std::string foreignFunctionName = function_attribute(parm, name, ast::FUNCTION,
                                                             interface, arguments, returnTypeName,
							     &text);
        std::string s = (implementation && !operatorName) ? parentName + "::" : "";
        parm.println(returnTypeName + " " + s + translatedName + interface + "{");
        parm.incIndent();
        if (!foreignFunctionName.empty()) {
          parm.println("// Foreign function call");
          parm.println("return " + foreignFunctionName + "(" + argumentNames + ");");
        }
        function_body(parm, f->body);
        parm.decIndent();
        parm.println("}");
        ascendHierarchy(parm);
      } else {
        database.addFunction(name, arguments, returnType, f);
        parm.println((operatorName ? "friend " : "") + returnTypeName + " " + translatedName + interface + ";");
      }
      debug.functionEnd("function_declarations");
    }
  }

  void SystemC::procedure_declarations(parameters& parm, ast::ProcedureDeclaration* f,
                                      bool implementation) {
    if (f) {
      debug.functionStart("procedure_declarations");
      printSourceLine(parm, f->name->text);
      std::string name = f->name->toString(true);
      ast::ObjectArguments arguments(true);
      generateObjectArguments(f->interface, arguments);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      if (f->body) {
        std::string parentName = database.getParentName();
        std::string foreignFunctionName = function_attribute(parm, name, ast::PROCEDURE,
                                                             interface, arguments, "void", &f->name->text);
        std::string s = implementation ? parentName + "::" : "";
        parm.println("void " + s + name + interface + "{");
        parm.incIndent();
        descendHierarchy(parm, name);
        if (!foreignFunctionName.empty()) {
          parm.println("// Foreign function call");
          parm.println(foreignFunctionName + "(" + argumentNames + ");");
        }
        procedure_body(parm, f->body);
        ascendHierarchy(parm);
        parm.decIndent();
        parm.println("}");
      } else {
        database.addProcedure(name, arguments, f);
        parm.println("void " + name + interface + ";");
      }
      debug.functionEnd("procedure_declarations");
    }
    }

  void SystemC::function_body(parameters& parm, ast::FunctionBody* f) {
    assert(f);
    debug.functionStart("function_body");
    declarations(parm, f->declarations);
    sequentialStatements(parm, f->sequentialStatements);
    debug.functionEnd("function_body");
  }
  
  void SystemC::procedure_body(parameters& parm, ast::ProcedureBody* f) {
    assert(f);
    debug.functionStart("procedure_body");
    declarations(parm, f->declarations);
    sequentialStatements(parm, f->sequentialStatements);
    debug.functionEnd("procedure_body");
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
	database.addAttribute(name, arguments, id, a, text);
      }
      debug.functionEnd("attribute_declarations");
    }
  }
      /*
  vhdl:
    subtype type_t is integer range 0 to 10;
  systemc:
    struct TYPE_T_range { int left = 0; int right = 4; };
    template<class T = TYPE_T_range>
    using TYPE_T = INTEGER<T>;
  */
  void SystemC::subtype_declarations(parameters& parm, ast::SubtypeDeclaration* t) {
    if (t) {
      std::string name = t->identifier->toString(true);
      DatabaseResult database_result;
      subtypeIndication(parm, database_result, name, t->type);
      database.add(ast::TYPE, name, database_result.object->type);
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d,
                             bool implementation) {
    debug.functionStart("declarations");
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      subtype_declarations(parm, i.subtype);
      object_declarations(parm, i.variable);
      object_declarations(parm, i.signal);
      object_declarations(parm, i.constant);
      function_declarations(parm, i.function, implementation);
      procedure_declarations(parm, i.procedure, implementation);
      attribute_declarations(parm, i.attribute);
    }
    debug.functionEnd("declarations");
  }

}

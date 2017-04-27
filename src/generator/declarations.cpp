#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"
#include "database/database.hpp"

namespace generator {

   void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
     if (t) {
       functionStart("type_declarations");
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
       functionEnd("type_declarations");
     }
  }

  bool SystemC::findType(ast::SimpleIdentifier* identifier, ast::ObjectValueContainer& type) {
    assert(identifier);
    std::string name = identifier->toString(true);
    auto valid = [&](DatabaseElement* e) {
      return e->id == ast::TYPE;
    };
    DatabaseResult object;
    if (database.findOne(object, name, valid)) {
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
        a.name = i.toString(true);
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
      return interfaceListToString(parm, interface, ", ", false,
                                   [](std::string& type, ast::ObjectType id,
                                      ast::ObjectDeclaration::Direction direction) {
                                     return type;
                                   });
    }
    return "";
  }
  
  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f,
                                      bool implementation) {
    if (f) {
      functionStart("function_declarations");
      std::string name = f->name->toString(true);
      ast::ObjectArguments arguments(true);
      generateObjectArguments(f->interface, arguments);
      {
        ast::ObjectValueContainer type;
        findType(f->returnType, type);
        database.addFunction(name, arguments, type, f);
      }
      descendHierarchy(parm, name);
      printSourceLine(parm, f->name->text);
      std::string returnType = f->returnType->toString(true) + "<>";
      std::string interface = "(" + getArgumentTypes(parm, f->interface) + ")";
      if (f->body) {
        std::string s = implementation ? database.getParentName() + "::" : "";
        println(parm, returnType + " " + s + name + interface + "{");
        parm.incIndent();
        function_body(parm, f->body);
        parm.decIndent();
        println(parm, "}");
      } else {
        println(parm, returnType + " " + name + interface + ";");
      }
      ascendHierarchy(parm);
      functionEnd("function_declarations");
    }
  }

  
  void SystemC::procedure_declarations(parameters& parm, ast::ProcedureDeclaration* f,
                                      bool implementation) {
    if (f) {
      functionStart("procedure_declarations");
      printSourceLine(parm, f->name->text);
      std::string name = f->name->toString(true);
      ast::ObjectArguments arguments(true);
      generateObjectArguments(f->interface, arguments);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      if (f->body) {
        std::string parentName = database.getParentName();
        auto valid = [&](DatabaseElement* e) {
          return (e->id == ast::PROCEDURE) && (e->arguments.equals(arguments));
        };
        std::string foreignFunctionName = "";
        DatabaseResult object;
        if (database.findOne(object, name, valid)) {
          DatabaseElement* e = object.object;
          if (e->attribute && e->attribute->expression) {
            println(parm, "/*");
            println(parm, " * This is the definition of the foreign function set as an attribute.");
            println(parm, " * The implementation must be defined in a .cpp file in this directory.");
            println(parm, "*/");
            foreignFunctionName = e->attribute->expression->toString(true);
            println(parm, "void " + foreignFunctionName + interface + ";");
          }
        } else {
          exceptions.printError("Did not find declaration of procedure \"" + name + "\"", &f->name->text); 
        }
        std::string s = implementation ? parentName + "::" : "";
        println(parm, "void " + s + name + interface + "{");
        parm.incIndent();
        descendHierarchy(parm, name);
        if (!foreignFunctionName.empty()) {
          println(parm, "// Foreign function call");
          println(parm, foreignFunctionName + "(" + argumentNames + ");");
        }
        procedure_body(parm, f->body);
        ascendHierarchy(parm);
        parm.decIndent();
        println(parm, "}");
      } else {
        database.addProcedure(name, arguments, f);
        println(parm, "void " + name + interface + ";");
      }
      functionEnd("procedure_declarations");
    }
    }

  void SystemC::function_body(parameters& parm, ast::FunctionBody* f) {
    assert(f);
    functionStart("function_body");
    declarations(parm, f->declarations);
    sequentialStatements(parm, f->sequentialStatements);
    functionEnd("function_body");
  }
  
  void SystemC::procedure_body(parameters& parm, ast::ProcedureBody* f) {
    assert(f);
    functionStart("procedure_body");
    declarations(parm, f->declarations);
    sequentialStatements(parm, f->sequentialStatements);
    functionEnd("procedure_body");
  }
  
  void SystemC::attribute_declarations(parameters& parm, ast::Attribute* a) {
    if (a) {
      functionStart("attribute_declarations");
      if (a->item) {
        std::string name = a->item->toString(true);
        ast::ObjectArguments arguments(true);
        generateObjectArguments(a->arguments, arguments);
        ast::ObjectType id = a->objectType;
        database.addAttribute(name, arguments, id, a, &a->item->text);
      }
      functionEnd("attribute_declarations");
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
      ast::ObjectValueContainer value;
      subtypeIndication(parm, value, name, t->type);
      database.add(ast::TYPE, name, value);
    }
  }

  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d,
                             bool implementation) {
    functionStart("declarations");
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
    functionEnd("declarations");
  }

}

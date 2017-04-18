#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"
#include "database.hpp"

namespace generator {

   void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      std::string name = t->identifier->toString(true);
      parm.database.addObject(name, ast::TYPE);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
      arrayType(parm, t->identifier, t->typeDefinition->arrayType);
    }
  }

  bool SystemC::findType(ast::SimpleIdentifier* name, ast::ObjectValueContainer& type) {
    assert(name);
    std::string name = name->toString(true);
    DatabaseResults objects;
    database.find(objects, name);
    bool typeFound = false;
    if (!objects.empty()) {
      for (DatabaseResult& o : objects) {
        if (o.object->id == ast::TYPE) {
          if (typeFound) {
            exceptions.printError("Found more than one type with name \"" + name + "\"", &name->text);
          }
          type = o.object->type;
          typeFound = true;
        }
      }
    }
    if (!typeFound) {
      exceptions.printError("Could not find declaration of type \"" + name + "\"", &name->text);
    }
    return typeFound;
  }
  
  void SystemC::generateObjectArguments(ast::InterfaceList* interface, ast::ObjectArguments& arguments) {
    if (interface) {
      for (ast::InterfaceElement& i : interface->interfaceElements.list) {
        ast::ObjectArgument a;
        a.name = i.object->identifier.toString(true);
        findType(i.object->type->name, a.type);
        a.defaultValue = (i.object->initialization) ? true : false;
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

  std::string SystemC::getArgumentNames(parameters& parm, ast::AssociationList* arguments) {
    if (arguments) {
      return listToString(parm, &arguments->associationElements, ", ",
                          [](ast::AssociationElements& e) {
                            return e.object->identifier->toString(true);
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
      ast::ObjectArguments arguments;
      generateObjectArguments(f->interface, arguments);
      ast::ObjectValueContainer returnType;
      findType(f->returnType, returnType);
      database.addFunction(name, arguments, returnType, f);
      descendHierarchy(parm, name);
      printSourceLine(parm, f->name->text);
      std::string returnType = f->returnType->toString(true) + "<>";
      std::string interface = "(" + argumentTypes + ")";
      if (f->body) {
        std::string s = implementation ? parm.database.getParentName() + "::" : "";
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

  bool SystemC::databaseFilter(DatabaseResults& objects, int number, ast::ObjectType type, ast::ObjectArguments& arguments, ast::Text* text) {
    DatabaseResults result;
    int matches = 0;
    for (DatabaseResult& o : objects) {
      if (type == o.object->type && ast::match(o.object->arguments, arguments)) {
        result.push_back(o);
        matches++;
      }
    }
    if (matches != number) {
      exceptions.printError("Expected " + std::to_string(number) + " matches, but found " + std::to_string(matches), text);
      return false;
    }
    objects = result;
    return true;
  }
  
  void SystemC::procedure_declarations(parameters& parm, ast::ProcedureDeclaration* f,
                                      bool implementation) {
    if (f) {
      functionStart("procedure_declarations");
      printSourceLine(parm, f->name->text);
      std::string name = f->name->toString(true);
      ast::ObjectArguments arguments;
      generateObjectArguments(f->interface, arguments);
      std::string argumentNames = getArgumentNames(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      if (f->body) {
        std::string parentName = parm.database.getParentName();
        DatabaseResults objects;
        database.find(objects, name);
        ;
        std::string foreignFunctionName = "";
        if (databaseFilter(objects, 1, ast::PROCEDURE, arguments, f->name->text)) {
          DatabaseElement* e = objects->begin()->object;
          if (e->attribute && e->attribute->expression) {
            println(parm, "/*");
            println(parm, " * This is the definition of the foreign function set as an attribute.");
            println(parm, " * The implementation must be defined in a .cpp file in this directory.");
            println(parm, "*/");
            foreignFunctionName = e->attribute->expression->toString(true);
            println(parm, "void " + foreignFunctionName + interface + ";");
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
        parm.database.addProcedure(name, arguments, f);
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
      if (a->item) {
        std::string name = a->item->toString(true);
        std::string arguments = getArgumentTypes(parm, a->arguments);
        ast::ObjectType id = a->objectType;
        parm.database.addAttribute(name, arguments, id, a);
      }
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
      subtypeIndication(parm, name, t->type);
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

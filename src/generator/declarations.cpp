#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"

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

  std::string SystemC::getArgumentTypes(parameters& parm, ast::InterfaceList* interface) {
    if (interface) {
      return listToString(parm, &interface->interfaceElements, ", ",
                          [](ast::InterfaceElement& e) {
                            return e.object->type->name->toString(true);
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
      std::string argumentTypes = getArgumentTypes(parm, f->interface);
      parm.database.addFunction(name, argumentTypes, f);
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

  void SystemC::procedure_declarations(parameters& parm, ast::ProcedureDeclaration* f,
                                      bool implementation) {
    if (f) {
      functionStart("procedure_declarations");
      printSourceLine(parm, f->name->text);
      std::string name = f->name->toString(true);
      std::string argumentTypes = getArgumentTypes(parm, f->interface);
      std::string interface = "(" + getInterface(parm, f->interface) + ")";
      if (f->body) {
        std::string s = implementation ? parm.database.getParentName() + "::" : "";
        DatabaseElement* e = parm.database.findObject(name, argumentTypes, ast::PROCEDURE);
        if (e && e->attribute && e->attribute->expression) {
          println(parm, "void " + e->attribute->expression->toString() + interface + ";");
        }
        println(parm, "void " + s + name + interface + "{");
        parm.incIndent();
        descendHierarchy(parm, name);
        procedure_body(parm, f->body);
        ascendHierarchy(parm);
        parm.decIndent();
        println(parm, "}");
      } else {
        parm.database.addProcedure(name, argumentTypes, f);
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
    println(parm, "// Declarations");
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

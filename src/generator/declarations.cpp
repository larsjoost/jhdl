#include <cassert>

#include "systemc.hpp"
#include "declarations.hpp"

namespace generator {

  void SystemC::addDeclarationType(parameters& parm, ast::SimpleIdentifier* identifier, DeclarationID id) {
      std::string name = identifier->toString(true);
      DeclarationInfo i;
      i.id = id;
      i.hierarchyLevel = 0;
      parm.declaration[name] = i;
  }
  
   void SystemC::type_declarations(parameters& parm, ast::TypeDeclaration* t) {
    if (t) {
      assert(t->typeDefinition);
      addDeclarationType(parm, t->identifier, TYPE);
      numberType(parm, t->identifier, t->typeDefinition->numberType);
      enumerationType(parm, t->identifier, t->typeDefinition->enumerationType);
      arrayType(parm, t->identifier, t->typeDefinition->arrayType);
    }
  }

  void SystemC::function_declarations(parameters& parm, ast::FunctionDeclaration* f,
                                      bool implementation) {
    if (f) {
      functionStart("function_declarations");
      addDeclarationType(parm, f->name, FUNCTION);
      std::string name = f->name->toString(true);
      parm.functions[name] = f;
      {
        parameters p = parm;
        printSourceLine(p, f->name->text);
        std::string returnType = f->returnType->toString(true) + "<>";
        std::string interface = "(" +
          interfaceListToString(p, f->interface, ", ", false,
                                [](std::string& type, DeclarationID id,
                                   ast::ObjectDeclaration::Direction direction) {
                                  return type;
                                }) + ")";
        if (f->body) {
          std::string s = implementation ? p.parentName + "::" : "";
          println(p, returnType + " " + s + name + interface + "{");
          p.incIndent();
          function_body(p, f->body);
          p.decIndent();
          println(p, "}");
        } else {
          println(p, returnType + " " + name + interface + ";");
        }
      }
      functionEnd("function_declarations");
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


  void SystemC::declarations(parameters& parm, ast::List<ast::Declaration>& d) {
    functionStart("declarations");
    println(parm, "// Declarations");
    for (ast::Declaration i : d.list) {
      type_declarations(parm, i.type);
      subtype_declarations(parm, i.subtype);
      object_declarations(parm, i.variable);
      object_declarations(parm, i.signal);
      object_declarations(parm, i.constant);
      function_declarations(parm, i.function, false);
    }
    functionEnd("declarations");
  }

}

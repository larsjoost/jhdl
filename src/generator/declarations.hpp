#include <cassert>

#include "systemc.hpp"

namespace generator {

  template<typename Func>
  void SystemC::objectDeclaration(parameters& parm, ast::ObjectDeclaration* v, Func callback) {
    if (v) {
      functionStart("objectDeclaration");
      assert(v->identifier);
      std::string name = v->identifier->toString(true);
      std::string type = name + "_type";
      type = subtypeIndication(parm, type, v->type) + "<>";
      std::string init = "";
      if (v->initialization) {
        init = expressionToString(parm, v->initialization->value);
      }
      DeclarationID id;
      switch (v->objectType) {
      case ast::ObjectDeclaration::SIGNAL: id = SIGNAL; break;
      case ast::ObjectDeclaration::VARIABLE: id = VARIABLE; break;
      case ast::ObjectDeclaration::CONSTANT: id = CONSTANT; break;
      default: assert(false);
      }
      addDeclarationType(parm, v->identifier, id);
      callback(name, type, init, id, v->direction);
      functionEnd("objectDeclaration");
    }
  }

}

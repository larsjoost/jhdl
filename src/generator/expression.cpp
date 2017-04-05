#include "systemc.hpp"
#include "expression.hpp"

namespace generator {

  std::string SystemC::physicalToString(parameters& parm, ast::Physical* p) {
    assert (p != NULL);
    return "{" + numberToString(parm, p->number) + ", " + p->unit->toString(true) + "}";
  }
  
  std::string SystemC::getNamePrefix(parameters& parm, ast::BasicIdentifier* identifier) {
    int hierarchyLevel;
    std::string id = identifier->toString(true);
    DatabaseElement* e = parm.database.findObject(id, hierarchyLevel);
    std::string s = "";
    if (e) {
      if (e->id == ast::ENUM || e->id == ast::TYPE) {
      } else if (hierarchyLevel >= 0) {
        for (int i=0; i<hierarchyLevel; i++) {
          s += "p->";
        }
      }
    }
    return s;
  }
  
  std::string SystemC::getName(parameters& parm, ast::BasicIdentifier* identifier, bool hierarchy) {
    functionStart("getName");
    assert (identifier != NULL);
    std::string s = "";
    std::string name = identifier->text.toString(true);
    if (hierarchy) {
      s = getNamePrefix(parm, identifier);
    }
    functionEnd("getName");
    return s + name;
  }
  
  std::string SystemC::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* identifier) {
    functionStart("basicIdentifierToString");
    assert (identifier);
    std::string name = identifier->text.toString(true);
    name = getNamePrefix(parm, identifier) + name;
    if (info.id == FUNCTION || info.id == PROCEDURE) {
      std::string parameters = parametersToString(parm, identifier, identifier->arguments);
      name += "(" + parameters + ")";
    } else {
      if (identifier->attribute) {
        bool objectMatch = (info.id == VARIABLE) || (info.id == SIGNAL);
        std::string seperator = objectMatch ? "." : "<>::";
        name += seperator + identifier->attribute->toString(true);
        std::string arguments = "";
        if (identifier->arguments) {
          arguments = listToString(parm, identifier->arguments->associationElements.list, ",",
                                   [&](ast::AssociationElement& a){return expressionToString(parm, a.actualPart);});
        }
        name += "(" + arguments + ")";
      } else if (identifier->arguments) {
        name += listToString(parm, identifier->arguments->associationElements.list, ",",
                             [&](ast::AssociationElement& a){return "[" + expressionToString(parm, a.actualPart) + "]";});
      }
    }
    functionEnd("basicIdentifierToString");
    return name;
  }

  std::string SystemC::numberToString(parameters& parm, ast::Number* n) {
    assert(n != NULL);
    return n->value.toString();
  }
  
  std::string SystemC::characterToString(parameters& parm, ast::Character* n) {
    assert(n != NULL);
    return n->toString();
  }


  std::string SystemC::expressionToString(parameters& parm, ast::Expression* e) {
    return expressionToString(parm, e, [](std::string baseName, std::string hierarchyName) {});
  }
  
}

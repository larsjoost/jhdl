#include "declarations.hpp"
#include "systemc.hpp"
#include "expression.hpp"

namespace generator {

  std::string SystemC::physicalToString(parameters& parm, ast::Physical* p) {
    assert (p);
    return "{" + numberToString(parm, p->number) + ", " + p->unit->toString(true) + "}";
  }
  
  DatabaseElement* SystemC::getName(parameters& parm, ast::BasicIdentifier* identifier, std::string& name) {
    functionStart("getName");
    assert (identifier);
    name = identifier->toString(true);
    std::string parentName = parm.database.getParentName();
    int hierarchyLevel;
    DatabaseElement* e = database.findObject(name, hierarchyLevel, parentName);
    if (e) {
      for (int i=0; i<hierarchyLevel; i++) {
        name = "p->" + name;
      }
    } else {
      printError("Did not find identifier " + name, &identifier->text);
    }
    functionEnd("getName");
    return e;
  }

  std::string SystemC::parametersToString(parameters& parm, ast::BasicIdentifier* identifier,
                                          ast::InterfaceList* interface,
                                          ast::AssociationList* associationList) {
    functionStart("parametersToString");
    std::string s = "";
    /*
      Association list can either be:
      func(formalPart => actualPart, a => w, b => x, c => y)
      or
      func(actualPart, w, x, y)
    */
    std::string delimiter = "";
    int argumentNumber = 0;
    auto func = [&](std::string& name,
                    std::string& type, std::string& init, ast::ObjectType id,
                    ast::ObjectDeclaration::Direction direction) {
      std::string argument = associateArgument(parm, name, init, argumentNumber, associationList);
      if (argument.size() == 0) {
        printError("No argument associated element " + std::to_string(argumentNumber), &identifier->text);
      }
      s += delimiter + argument;
      delimiter = ", ";
      argumentNumber++;
    };
    traverseInterfaceList(parm, interface, false, func);
    functionEnd("parametersToString");
    return s;
  }

  std::string SystemC::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* identifier) {
    functionStart("basicIdentifierToString");
    std::string name = "";
    DatabaseElement* e = getName(parm, identifier, name);
    if (e) {
      if (e->id == ast::FUNCTION || e->id == ast::PROCEDURE) {
        ast::InterfaceList* l = (e->id == ast::FUNCTION) ? e->function->interface : e->procedure->interface;
        std::string parameters = parametersToString(parm, identifier, l, identifier->arguments);
        name += "(" + parameters + ")";
      } else {
        if (identifier->attribute) {
          bool objectMatch = (e->id == ast::VARIABLE) || (e->id == ast::SIGNAL);
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
    } else {
      printError("Could not find identifier " + identifier->toString(true), &identifier->text);
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

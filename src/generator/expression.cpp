#include "systemc.hpp"
#include "expression.hpp"

namespace generator {

  std::string SystemC::physicalToString(parameters& parm, ast::Physical* p) {
    assert (p != NULL);
    return "TIME<>(" + numberToString(parm, p->number) + ", " +
      basicIdentifierToString(parm, p->unit) + ")";
  }
  
  bool SystemC::getIdentifierInfo(parameters& parm, ast::BasicIdentifier* identifier, IdentifierInfo& info) {
    functionStart("getIdentifierInfo");
    assert (identifier);
    std::string name = identifier->text.toString(true);
    auto t = parm.declaration.find(name); 
    bool result = true;
    if (t != parm.declaration.end()) {
      info.id = t->second.id;
      info.packageName = "WORK";
      info.hierarchyLevel = t->second.hierarchyLevel;
    } else {
      auto t = visiblePackageInfo.find(name);
      if (t != visiblePackageInfo.end()) {
        info.packageName = t->second.name;
        info.id = t->second.id;
        info.hierarchyLevel = -1;
      } else {
        result = false;
        std::string s = "";
        for (auto i : packageInfo) {
          auto x = i.second.find(name);
          if (x != i.second.end()) {
            s = ". Found " + name + " in package " + i.first + ". Maybe you forgot to declare it in an USE statement.";
          }
        }
        printError(identifier->text, "Could not find declaration of identifier " + name + s);
      }
    }
    functionEnd("getIdentifierInfo");
    return result;
  }
  
  std::string SystemC::getNamePrefix(IdentifierInfo& info) {
    std::string s = "";
    if (info.id == ENUM || info.id == TYPE) {
    } else if (info.hierarchyLevel >= 0) {
      for (int i=0; i<info.hierarchyLevel; i++) {
        s += "p->";
      }
    } else {
      s = info.packageName + ".";
    }
    return s;
  }
  
  std::string SystemC::getName(parameters& parm, ast::BasicIdentifier* identifier, bool hierarchy) {
    functionStart("getName");
    assert (identifier != NULL);
    std::string s = "";
    std::string name = identifier->text.toString(true);
    if (hierarchy) {
      IdentifierInfo info;
      if (getIdentifierInfo(parm, identifier, info)) {
        s = getNamePrefix(info);
      }
    }
    functionEnd("getName");
    return s + name;
  }
  
  std::string SystemC::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* identifier) {
    functionStart("basicIdentifierToString");
    assert (identifier);
    IdentifierInfo info;
    std::string name = identifier->text.toString(true);
    if (getIdentifierInfo(parm, identifier, info)) {
      name = getNamePrefix(info) + name;
      if (info.id == FUNCTION) {
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

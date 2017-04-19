#include "expression.hpp"

namespace generator {

  std::string ExpressionParser::associateArgument(std::string& name, std::string& init,
                                                  int argumentNumber, ast::AssociationList* l) { 
    //    functionStart("associateArgument");
    std::string argument = init;
    if (l) {
      int associationElementNumber = 0;
      for (auto& e : l->associationElements.list) {
        std::string actualPart = expressionToString(e.actualPart);
        if (e.formalPart) {
          std::string formalPart = e.formalPart->toString(true);
          if (formalPart == name) {
            return actualPart;
          }
        } else if (associationElementNumber == argumentNumber) {
          return actualPart;
        }
        associationElementNumber++;
      }
    }
    // functionEnd("associateArgument");
    return argument;
  }

  std::string ExpressionParser::parametersToString(ast::InterfaceList* interface,
                                                   ast::AssociationList* associationList) {
    // functionStart("parametersToString");
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
      std::string argument = associateArgument(name, init, argumentNumber, associationList);
      if (argument.size() == 0) {
        exceptions.printError("No argument associated element " + std::to_string(argumentNumber), &identifier->text);
      }
      s += delimiter + argument;
      delimiter = ", ";
      argumentNumber++;
    };
    traverseInterfaceList(parm, interface, false, func);
    // functionEnd("parametersToString");
    return s;
  }

  std::string ExpressionParser::bestMatchesToString(DatabaseResult& object,
                                                    std::string name, 
                                                    ast::ObjectArguments* arguments) {
    assert(bestMatch.object);
    if (bestMatch.local) {
      for (int i=0; i < bestMatch.hierarchyLevel; i++) {
        name = "p->" + name;
      }
    } else {
      name =
        bestMatch.object->library + "::" +
        bestMatch.object->package + "::" +
        name;
    }
    if (arguments) {
      assert(object.id == ast::FUNCTION || object.id == ast::PROCEDURE);
      ast::InterfaceList* l = (object.id == ast::FUNCTION) ? object.function->interface : object.procedure->interface;
      std::string parameters = parametersToString(l, identifier->arguments);
      name += "(" + parameters + ")";
    }
    return name;
  }

  ast::ObjectValueContainer ExpressionParser::getAttributeType(ast::ObjectValueContainer& type,
                                                               std::string attributeName) {
    static std::unordered_map<std::string, ast::ObjectValueContainer> t =
      {{"IMAGE", ast::ObjectValueContainer(ast::USER_TYPE, "STRING")},
       {"LENGTH", ast::ObjectValueContainer(ast::INTEGER)}};
    auto i = t.find(attributeName);
    if (i != t.end()) {
      return i->second;
    } else {
      
    }
  }
  
  void ExpressionParser::findAttributeMatch(DatabaseResults& objects,
                                            DatabaseResult& match,
                                            std::string& name) {
    match.object = NULL;
    for (auto& i : objects) {
      ast::ObjectValueContainer a = getAttributeType(i.object->type, name);
      if (expectedType.equals(a)) {
        if (!bestMatch.object) {
          bestMatch = i;
        } else {
          exception.printError("Found more than one attribute match");
        }
      }
    }
  }
  
  std::string ExpressionParser::attributeToString(DatabaseResults& object,
                                                  ast::ObjectValueContainer& expectedType,
                                                  ast::Attribute* attribute,
                                                  ast::ObjectArguments* arguments) {
    assert(attribute);
    std::string name = attribute->toString(true);
    DatabaseResult match;
    findAttributeMatch(objects, match, name);
    if (match.object) {
      bool objectMatch = (match.object->id == ast::VARIABLE) || (match.object->id == ast::SIGNAL);
      std::string seperator = objectMatch ? "." : "<>::";
      name = seperator + name;
      if (arguments) {
        std::string a = "";
        std::string delimiter = "";
        for (auto& i : arguments->associationElements.list) {
          a = delimiter + expressionToString(i.actualPart);
          delimiter = ", ";
        }
        name += "(" + a + ")";
      }
    } else {
      exceptions.printError("Cound not find match for attribute " + name);
    }
    return name;
  }
      
  template <typename Func>
  std::string ExpressionParser::basicIdentifierToString(ast::BasicIdentifier* b,
                                                        ast::ObjectValueContainer& expectedType,
                                                        Func sensitivityListCallback) {
    // functionStart("basicIdentifierToString");
    std::string name = b->indentifier->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(identifier->arguments);
    auto valid = [&](DatabaseElement* e) {
      if (e->arguments.equals(arguments) &&
          e->type.equals(expectedType)) {
        return true;
      } else {
        return false;
      }
    };
    DatabaseResult object;
    if (database.find(object, name, valid)) {
      if (identifier->attribute) {
        return attributeToString(object, expectedType, identifier->attribute, identifier->arguments);
      } else {
        if (object.object->id == ast::SIGNAL) {
          sensitivityListCallback(name);
        }
        return databaseResultToString(bestMatch, b->arguments);
      }
    } else {
      expections.printError("Could not find definition of name \"" + name + "\"", &b->identifier->text);
    }
    // functionEnd("basicIdentifierToString");
    return name;
  }

}

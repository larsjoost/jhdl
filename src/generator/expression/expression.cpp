#include "expression.hpp"

namespace generator {

  void ExpressionParser::functionStart(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION START] " << name << std::endl;
    }
  }

  void ExpressionParser::functionEnd(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION END] " << name << std::endl;
    }
  }

  std::string ExpressionParser::returnTypesToString(ReturnTypes& returnTypes) {
    std::string found = "";
    std::string delimiter;
    for (auto& i : returnTypes) {
      found += delimiter + i.toString();
      delimiter = ", ";
    }
    return found;
  }
  
  bool ExpressionParser::getType(ast::Expression* e,
                                 ast::ObjectValueContainer& expectedType,
                                 ast::ObjectValueContainer& actualType) {
    bool result;
    ReturnTypes o = expressionReturnTypes(e);
    if (o.size() == 1 && o.back().equals(expectedType)) {
      actualType = o.back();
      result = true;
    } else {
      exceptions.printError("Expected " + expectedType.toString() +
                            ", but found " + returnTypesToString(o), e->text);
      result = false;
    }
    return result;
  }
  
  std::string ExpressionParser::toString(ast::Expression* e, ast::ObjectValueContainer& expectedType) {
    functionStart("toString");
    std::string s = toString(e, expectedType, [&](DatabaseResult& object) {});
    functionEnd("toString");
    return s;
  }

  std::string ExpressionParser::toString(ast::Expression* expr, ast::ObjectValue expectedType) {
    functionStart("toString");
    ast::ObjectValueContainer e(expectedType);
    std::string s = toString(expr, e);
    functionEnd("toString");
    return s;
  }


  std::string ExpressionParser::procedureCallStatementToString(ast::ProcedureCallStatement* p) {
    functionStart("procedureCallStatementToString");
    assert(p);
    std::string name = p->name->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(p->arguments);
    auto valid = [&](DatabaseElement* e) {
      return e->id == ast::PROCEDURE && e->arguments.equals(arguments);
    };
    DatabaseResult object;
    if (database->findOne(object, name, valid)) {
      return objectToString(object, p->arguments, [&](DatabaseResult& e) {});
    } else {
      exceptions.printError("Could not find definition of procedure \"" + name + "\"", &p->name->text);
      database->printAllObjects(name);
    }
    functionEnd("procedureCallStatementToString");
    return name;
  }

  bool ExpressionParser::translateOperator(std::string& op, std::string& translatedOp) {
    static std::unordered_map<std::string, std::string> translate =
      { {"/=", "!="}, {"=", "=="}, {">=", ">="}, {"+", "+"}, {"-", "-"}, {"&", "+"} };
    auto i = translate.find(op);
    bool result = false;
    if (i != translate.end()) {
      translatedOp = i->second;
      result = true;
    }
    return result;
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::operatorReturnTypes(std::string& name,
                                                                      ast::ObjectValueContainer& l,
                                                                      ast::ObjectValueContainer& r) {
    std::list<ast::ObjectArgument> a = {ast::ObjectArgument(l), ast::ObjectArgument(r)};
    auto x = ast::ObjectArguments(false, a);
    auto valid = [&](DatabaseElement* e) {
      return x.equals(e->arguments);
    };
    return getReturnTypes(name, valid);
  }

  ExpressionParser::ReturnTypes ExpressionParser::expressionReturnTypes(ast::Expression* e) {
    functionStart("expressionReturnTypes");
    assert(e);
    if (e->parenthis) {
      e->returnTypes = expressionReturnTypes(e->parenthis);
    } else if (e->unaryOperator) {
      e->returnTypes = expressionReturnTypes(e->expression);
    } else if (e->op) {
      ReturnTypes expr = expressionReturnTypes(e->expression);
      ReturnTypes term = expressionTermReturnTypes(e->term);
      for (auto& i : term) {
        for (auto& j : expr) {
          ReturnTypes t = operatorReturnTypes(e->op->op, i, j);
          e->returnTypes.insert(e->returnTypes.end(), t.begin(), t.end());
        }
      }
      if (e->returnTypes.empty()) {
        exceptions.printError("Could not resolve type of expression \"" +
                              returnTypesToString(expr) + "\" " + e->op->op + " \"" +
                              returnTypesToString(term) + "\"", e->text);
      }
    } else {
      e->returnTypes = expressionTermReturnTypes(e->term);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression", e->text);
    }
    functionEnd("expressionReturnTypes");
    return e->returnTypes;
  }

  std::string ExpressionParser::physicalToString(ast::Physical* physical) {

    std::string enumName = physical->unit->toString(true);
    database->globalName(enumName, ast::ENUM);
    return "{" + physical->number->toString() + ", " + enumName + "}";
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::expressionTermReturnTypes(ast::ExpressionTerm* e) {
    functionStart("expressionTermReturnTypes");
    assert(e);
    if (e->physical) {
      e->returnTypes = {ast::ObjectValueContainer(ast::PHYSICAL, e->physical->unit->toString(true))};
    } else if (e->number) {
      e->returnTypes = {ast::ObjectValueContainer(e->number->type)};
    } else if (e->string) {
      static ast::ObjectValueContainer stringType = database->getType("STRING", "STANDARD", "STD");
      e->returnTypes = {stringType};
    } else if (e->identifier) {
      e->returnTypes = basicIdentifierReturnTypes(e->identifier);
    } else if (e->character) {
      e->returnTypes = {ast::ObjectValueContainer(ast::CHARACTER)};
    } else {
      exceptions.printInternal("Unknown expression term", e->text);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression term", e->text);
    }
    functionStart("expressionTermReturnTypes");
    return e->returnTypes;
  }

  ast::ObjectArguments ExpressionParser::toObjectArguments(ast::AssociationList* associationList) {
    ast::ObjectArguments result(false);
    if (associationList) {
      for (auto& i : associationList->associationElements.list) {
        ast::ObjectArgument x;
        x.name = (i.formalPart && i.formalPart->name) ? i.formalPart->name->toString(true) : "";
        ReturnTypes r = expressionReturnTypes(i.actualPart);
        if (r.size() == 1) {
          x.type = *(r.begin());
          result.push_back(x);
        } else {
          if (r.empty()) {
            exceptions.printError("Could not resolve argument type", &i.formalPart->name->text);
          } else {
            exceptions.printError("More than one type for argument", &i.formalPart->name->text);
          }
        }
      }
    }
    return result;
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::functionReturnTypes(std::string& name, ast::AssociationList* associationList) {
    ast::ObjectArguments a = toObjectArguments(associationList);
    
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::attributeReturnTypes(std::string& name, std::string& attribute,
                                                                       ast::AssociationList* associationList) {
    functionStart("attributeReturnTypes");
    ReturnTypes result;
    // ast::ObjectArguments arguments = toObjectArguments(associationList);
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    DatabaseResult object;
    if (database->findOne(object, name, valid)) {
      ast::ObjectValueContainer type = getAttributeType(object.object->type, attribute);
      result.push_back(type);
    } else {
      exceptions.printError("Could not resolve type of attribute \"" + attribute + "\" of \"" + name + "\"");
    }
    functionEnd("attributeReturnTypes");
    return result;
  }

  ExpressionParser::ReturnTypes ExpressionParser::basicIdentifierReturnTypes(ast::BasicIdentifier* b) {
    functionStart("basicIdentifierReturnTypes");
    std::string name = b->text.toString(true);
    if (b->attribute) {
      std::string attribute = b->attribute->toString(true);
      b->returnTypes = attributeReturnTypes(name, attribute, b->arguments);
    } else if (b->arguments) {
      b->returnTypes = functionReturnTypes(name, b->arguments); 
    } else {
      auto valid = [&](DatabaseElement* e) {
        return e->arguments.empty();
      };
      b->returnTypes = getReturnTypes(name, valid);
    }
    if (b->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of " + name, &b->text);
    }
    functionEnd("basicIdentifierReturnTypes");
    return b->returnTypes;
  }

  bool ExpressionParser::getStaticAttributeType(std::string attributeName, ast::ObjectValueContainer& result) {
    static ast::ObjectValueContainer stringType = database->getType("STRING", "STANDARD", "STD");
    static std::unordered_map<std::string, ast::ObjectValueContainer> fixedAttributeTypes =
      {{"IMAGE", stringType},
       {"LENGTH", ast::ObjectValueContainer(ast::INTEGER)}};
    bool found = false;
    auto i = fixedAttributeTypes.find(attributeName);
    if (i != fixedAttributeTypes.end()) {
      result = i->second;
      found = true;
    }
    return found;
  }
  
  ast::ObjectValueContainer ExpressionParser::getAttributeType(ast::ObjectValueContainer& type,
                                                               std::string attributeName) {
    ast::ObjectValueContainer result(ast::UNKNOWN);
    if (!getStaticAttributeType(attributeName, result)) {
      if (attributeName == "HIGH" || attributeName == "LOW") {
        switch(type.value) {
        case ast::INTEGER: result = ast::ObjectValueContainer(ast::INTEGER); break;
        case ast::PHYSICAL: result = ast::ObjectValueContainer(ast::PHYSICAL); break;
        default: exceptions.printError("Could not find attribute \"" + attributeName + "\" of type " + type.toString()); 
        };
      } else {
        exceptions.printError("Could not resolve type of attribute " + attributeName);
      }
    }
    return result;
  }
  
  bool ExpressionParser::findAttributeMatch(DatabaseResults& objects,
                                            DatabaseResult& match,
                                            ast::ObjectValueContainer& expectedType,
                                            std::string& name) {
    bool foundMatch = false;;
    for (auto& i : objects) {
      ast::ObjectValueContainer a = getAttributeType(i.object->type, name);
      if (expectedType.equals(a)) {
        if (!foundMatch) {
          match = i;
          foundMatch = true;
        } else {
          exceptions.printError("Found more than one attribute match on " + name);
          exceptions.printError("Last match = " + match.toString());
          exceptions.printError("New match = " + i.toString());
        }
      }
    }
    return foundMatch;
  }
  
      

}

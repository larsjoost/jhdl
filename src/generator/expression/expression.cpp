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
      name = name + "(" + objectToString(object, p->arguments, [&](DatabaseResult& e) {}) + ")";
    } else {
      exceptions.printError("Could not find definition of procedure \"" + name + "\"", &p->name->text);
    }
    functionEnd("procedureCallStatementToString");
    return name;
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
    ReturnTypes result;
    if (e->parenthis) {
      result = expressionReturnTypes(e->parenthis);
    } else if (e->unaryOperator) {
      result = expressionReturnTypes(e->expression);
    } else if (e->op) {
      ReturnTypes term = expressionTermReturnTypes(e->term);
      ReturnTypes expr = expressionReturnTypes(e->expression);
      ReturnTypes result;
      for (auto& i : term) {
        for (auto& j : expr) {
          ReturnTypes t = operatorReturnTypes(e->op->op, i, j);
          result.insert(result.end(), t.begin(), t.end());
        }
      }
    } else {
      result = expressionTermReturnTypes(e->term);
    }
    functionEnd("expressionReturnTypes");
    return result;
  }

  ExpressionParser::ReturnTypes ExpressionParser::expressionTermReturnTypes(ast::ExpressionTerm* e) {
    functionStart("expressionTermReturnTypes");
    assert(e);
    ReturnTypes result;
    if (e->physical) {
      result = {ast::ObjectValueContainer(ast::PHYSICAL, e->physical->unit->toString(true))};
    } else if (e->number) {
      result = {ast::ObjectValueContainer(e->number->type)};
    } else if (e->text) {
      result = {ast::ObjectValueContainer(ast::TEXT)};
    } else if (e->identifier) {
      result = basicIdentifierReturnTypes(e->identifier);
    } else if (e->character) {
      result = {ast::ObjectValueContainer(ast::CHARACTER)};
    } else {
      exceptions.printInternal("Unknown expression term");
      assert(false);
    }
    functionStart("expressionTermReturnTypes");
    return result;
  }

  ast::ObjectArguments ExpressionParser::toObjectArguments(ast::AssociationList* associationList) {
    auto a = ast::ObjectArguments(false);
    if (associationList) {
      for (auto& i : associationList->associationElements.list) {
        ast::ObjectArgument x;
        x.name = i.formalPart->name->toString(true);
        ReturnTypes r = expressionReturnTypes(i.actualPart);
        if (r.size() == 1) {
          x.type = *(r.begin());
        } else {
          if (r.empty()) {
            exceptions.printError("Could not resolve argument type", &i.formalPart->name->text);
          } else {
            exceptions.printError("More than one type for argument", &i.formalPart->name->text);
          }
        }
      }
    }
    return a;
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::functionReturnTypes(std::string& name, ast::AssociationList* associationList) {
    ast::ObjectArguments a = toObjectArguments(associationList);
    
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::attributeReturnTypes(std::string& name, std::string& attribute,
                                                                       ast::AssociationList* associationList) {
    functionStart("attributeReturnTypes");
    ReturnTypes result;
    ast::ObjectArguments arguments = toObjectArguments(associationList);
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    DatabaseResult object;
    if (database->findOne(object, name, valid)) {
      result.push_back(object.object->type);
    } else {
      exceptions.printError("Could not resolve type of attribute \"" + attribute + "\" of \"" + name + "\"");
    }
    functionEnd("attributeReturnTypes");
    return result;
  }

  ExpressionParser::ReturnTypes ExpressionParser::basicIdentifierReturnTypes(ast::BasicIdentifier* b) {
    functionStart("basicIdentifierReturnTypes");
    ReturnTypes result;
    std::string name = b->text.toString(true);
    if (b->attribute) {
      std::string attribute = b->attribute->toString(true);
      result = attributeReturnTypes(name, attribute, b->arguments);
    } else if (b->arguments) {
      result = functionReturnTypes(name, b->arguments); 
    } else {
      auto valid = [&](DatabaseElement* e) {
        return e->arguments.empty();
      };
      result = getReturnTypes(name, valid);
    }
    functionEnd("basicIdentifierReturnTypes");
    return result;
  }
 
  ast::ObjectValueContainer ExpressionParser::getAttributeType(ast::ObjectValueContainer& type,
                                                               std::string attributeName) {
    static std::unordered_map<std::string, ast::ObjectValueContainer> fixedAttributeTypes =
      {{"IMAGE", ast::ObjectValueContainer(ast::USER_TYPE, "STRING")},
       {"LENGTH", ast::ObjectValueContainer(ast::INTEGER)}};
    auto i = fixedAttributeTypes.find(attributeName);
    if (i != fixedAttributeTypes.end()) {
      return i->second;
    } else {
      if (attributeName == "HIGH") {
        switch(type.value) {
        case ast::INTEGER: return ast::ObjectValueContainer(ast::INTEGER); break;
        case ast::PHYSICAL: return ast::ObjectValueContainer(ast::PHYSICAL); break;
        default: exceptions.printError("Could not find attribute \"HIGH\" of type " + type.toString()); 
        };
      } else {
        exceptions.printError("Could not resolve type of attribute " + attributeName);
        assert(false);
      }
    }
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

#include "expression.hpp"

namespace generator {

  std::string ExpressionParser::toString(ast::Expression* e, ast::ObjectValueContainer& expectedType) {
    return toString(e, expectedType, [&](DatabaseResult& object) {});
  }

  std::string ExpressionParser::toString(ast::Expression* expr, ast::ObjectValue expectedType) {
    ast::ObjectValueContainer e = ast::ObjectValueContainer(expectedType);
    return toString(expr, e);
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
    assert(e);
    if (e->parenthis) {
      expressionReturnTypes(e->parenthis);
    } else if (e->unaryOperator) {
      return expressionReturnTypes(e->expression);
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
      return result;
    } else {
      return expressionTermReturnTypes(e->term);
    }
    assert(false);
  }

  ExpressionParser::ReturnTypes ExpressionParser::expressionTermReturnTypes(ast::ExpressionTerm* e) {
    assert(e);
    if (e->physical) {
      return {ast::ObjectValueContainer(ast::PHYSICAL, e->physical->unit->toString(true))};
    }
    if (e->number) {
      return {ast::ObjectValueContainer(e->number->type)};
    }
    if (e->text) {
      return {ast::ObjectValueContainer(ast::TEXT)};
    }
    if (e->identifier) {
      return basicIdentifierReturnTypes(e->identifier);
    }
    if (e->character) {
      return {ast::ObjectValueContainer(ast::CHARACTER)};
    }
    assert(false);
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
    ast::ObjectArguments arguments = toObjectArguments(associationList);
    
  }

  ExpressionParser::ReturnTypes ExpressionParser::basicIdentifierReturnTypes(ast::BasicIdentifier* b) {
    std::string name = b->text.toString(true);
    if (b->attribute) {
      std::string attribute = b->attribute->toString(true);
      return attributeReturnTypes(name, attribute, b->arguments);
    } else if (b->arguments) {
      return functionReturnTypes(name, b->arguments); 
    } else {
      auto valid = [&](DatabaseElement* e) {
        return e->arguments.empty();
      };
      return getReturnTypes(name, valid);
    }
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
      exceptions.printError("Could not resolve type of attribute " + attributeName);
      assert(false);
    }
  }
  
  void ExpressionParser::findAttributeMatch(DatabaseResults& objects,
                                            DatabaseResult& match,
                                            ast::ObjectValueContainer& expectedType,
                                            std::string& name) {
    match.object = NULL;
    for (auto& i : objects) {
      ast::ObjectValueContainer a = getAttributeType(i.object->type, name);
      if (expectedType.equals(a)) {
        if (!match.object) {
          match = i;
        } else {
          exceptions.printError("Found more than one attribute match");
        }
      }
    }
  }
  
      

}

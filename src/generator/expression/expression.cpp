#include <algorithm>

#include "expression.hpp"

namespace generator {

  std::string ExpressionParser::returnTypesToString(ReturnTypes& returnTypes) {
    debug.functionStart("returnTypesToString", true);
    std::string found = "";
    std::string delimiter;
    for (auto& i : returnTypes) {
      found += delimiter + i.toString();
      delimiter = ", ";
    }
    debug.debug("Result = " + found, true);
    debug.functionEnd("returnTypesToString");
    return found;
  }

  bool ExpressionParser::exists(ReturnTypePair& pair, std::list<ReturnTypePair>& typePairs) {
    for (auto& i : typePairs) {
      if ((i.left.equals(pair.left) && i.right.equals(pair.right)) ||
          (i.left.equals(pair.right) && i.right.equals(pair.left))) {
        return true;
      }
    }
    return false;
  }
  
  bool ExpressionParser::getType(ast::Expression* e,
                                 ast::ObjectValueContainer& expectedType,
                                 ast::ObjectValueContainer& actualType) {
    debug.functionStart("getType");
    bool ok = false;
    ReturnTypes o = expressionReturnTypes(e);
    for (auto i : o) {
      if (i.equals(expectedType)) {
        ok = true;
        actualType = i;
        break;
      }
    }
    if (!ok) {
      std::string found = returnTypesToString(o);
      if (o.size() > 1) {
        exceptions.printError("Could not resolve expected type " + expectedType.toString() +
                              ". Found the following types: " + found, e->text);
      } else {
        actualType = o.back();
      }
    }
    debug.functionEnd("getType");
    return ok;
  }
  
  std::string ExpressionParser::toString(ast::Expression* e, ast::ObjectValueContainer& expectedType) {
    debug.functionStart("toString");
    std::string s = toString(e, expectedType, [&](DatabaseResult& object) {});
    debug.functionEnd("toString");
    return s;
  }

  std::string ExpressionParser::toString(ast::Expression* expr, ast::ObjectValue expectedType) {
    debug.functionStart("toString");
    ast::ObjectValueContainer e(expectedType);
    std::string s = toString(expr, e);
    debug.functionEnd("toString");
    return s;
  }


  std::string ExpressionParser::procedureCallStatementToString(ast::ProcedureCallStatement* p) {
    debug.functionStart("procedureCallStatementToString");
    assert(p);
    std::string name = p->name->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(p->arguments);
    auto valid = [&](DatabaseElement* e) {
      bool result = ((e->id == ast::ObjectType::PROCEDURE) &&
                     e->arguments.equals(arguments));
      debug.debug(e->toString() + (result ? " == " : " != ") + name + "(" + arguments.toString() + ")",
                  true, (result ? Output::Color::GREEN : Output::Color::RED));
      return result;
    };
    DatabaseResult object;
    if (a_database->findOne(object, name, valid)) {
      name = objectToString(object, p->arguments, [&](DatabaseResult& e) {});
    } else {
      exceptions.printError("Could not find definition of procedure \"" + name + "\"", &p->name->text);
      a_database->printAllObjects(name);
    }
    debug.functionEnd("procedureCallStatementToString");
    return name;
  }

  void ExpressionParser::removeDuplicates(ReturnTypes& r) {
    auto equal = [](ast::ObjectValueContainer& l, ast::ObjectValueContainer& r) {
      return l.equals(r);
    };
    auto it = std::unique(r.begin(), r.end(), equal);
    r.erase(it, r.end());
  }
  
  bool ExpressionParser::objectWithArguments(DatabaseElement* e, ast::ObjectArguments& arguments,
                                             ast::ObjectValueContainer* expectedReturnType) {
    debug.functionStart("objectWithArguments");
    static ast::ObjectValueContainer arrayType(ast::ObjectValue::ARRAY);
    bool result;
    if (e->id == ast::ObjectType::FUNCTION && !e->arguments.equals(arguments)) {
      result = false;
    } else {
      // TODO: Check Array arguments
      if (expectedReturnType) {
        ast::ObjectValueContainer* type;
        if (e->type.equals(arrayType)) {
          type = e->type.subtype;
        } else {
          type = &e->type;
        }
        assert(type);
        result = type->equals(*expectedReturnType); 
        debug.debug("Return type " + type->toString() + (result ? " == " : " != ") +
                    expectedReturnType->toString());
      } else {
        result = true;
      }
    }
    debug.debug("e = " + e->toString());
    debug.functionEnd("objectWithArguments = " + std::string(result ? "true" : "false"));
    return result;
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::functionReturnTypes(std::string& name,
                                                                      ast::AssociationList* associationList) {
    debug.functionStart("functionReturnTypes");
    assert(associationList);
    ReturnTypes result;
    ast::ObjectArguments arguments = toObjectArguments(associationList);
    auto valid = [&](DatabaseElement* e) {
      return objectWithArguments(e, arguments);
    };
    DatabaseResults objects;
    a_database->findAll(objects, name, valid);
    for (auto& i : objects) {
      ast::ObjectValueContainer* subtype = i.object->type.subtype;
      if (subtype) {
        result.push_back(*subtype);
      } else {
        result.push_back(i.object->type);
      }
    }
    debug.functionEnd("functionReturnTypes");
    return result;
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
  
  ExpressionParser::ReturnTypes ExpressionParser::getStandardOperatorReturnTypes(std::string& name,
										 ast::ObjectValueContainer& l,
										 ast::ObjectValueContainer& r) {
    struct Map {
      ast::ObjectValue l;
      ast::ObjectValue r;
      ast::ObjectValue result;};
    const static ast::ObjectValue BOOLEAN = ast::ObjectValue::BOOLEAN;
    const static ast::ObjectValue ARRAY = ast::ObjectValue::ARRAY;
    const static ast::ObjectValue DONT_CARE = ast::ObjectValue::DONT_CARE;
    static std::unordered_map<std::string, Map> translate =
      { {"&", {ARRAY, ARRAY, ARRAY}},
	{"+", {DONT_CARE, DONT_CARE, DONT_CARE}},
	{"-", {DONT_CARE, DONT_CARE, DONT_CARE}},
	{"=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{"/=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{"<=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{">=", {DONT_CARE, DONT_CARE, BOOLEAN}}
      };
    ReturnTypes result;
    if (l.value == r.value) {
      auto i = translate.find(name);
      if (i != translate.end()) {
	Map& m = i->second;
	if ((m.l == DONT_CARE || m.l == l.value) &&
	    (m.r == DONT_CARE || m.r == r.value) &&
            l.equals(r)) {
	  ast::ObjectValue v;
	  if (m.result == DONT_CARE) {
	    v = l.value;
	  } else {
	    v = m.result;
	  }
	  ast::ObjectValueContainer x(v);
	  result.push_back(v);
	}
      }
    }
    return result;
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::operatorReturnTypes(std::string& name,
                                                                      ast::ObjectValueContainer& l,
                                                                      ast::ObjectValueContainer& r) {
    debug.functionStart("operatorReturnTypes");
    std::list<ast::ObjectArgument> a = {ast::ObjectArgument(l), ast::ObjectArgument(r)};
    auto x = ast::ObjectArguments(false, a);
    auto valid = [&](DatabaseElement* e) {
      return x.equals(e->arguments);
    };
    ReturnTypes t = getReturnTypes(name, valid);
    ReturnTypes s = getStandardOperatorReturnTypes(name, l, r);
    t.insert(t.end(), s.begin(), s.end());
    debug.functionEnd("operatorReturnTypes");
    return t;
  }

  ExpressionParser::ReturnTypes ExpressionParser::expressionReturnTypes(ast::Expression* e) {
    debug.functionStart("expressionReturnTypes");
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
    debug.functionEnd("expressionReturnTypes");
    return e->returnTypes;
  }

  std::string ExpressionParser::physicalToString(ast::Physical* physical) {
    debug.functionStart("physicalToString");
    std::string enumName = a_name_converter->GetName(physical->unit, ast::ObjectType::ENUM);
    std::string result = "{" + physical->number->toString() + ", " + enumName + "}";
    debug.functionEnd("physicalToString");
    return result;
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::expressionTermReturnTypes(ast::ExpressionTerm* e) {
    debug.functionStart("expressionTermReturnTypes");
    assert(e);
    if (e->physical) {
      e->returnTypes = {ast::ObjectValueContainer(ast::ObjectValue::PHYSICAL, e->physical->unit->toString(true))};
    } else if (e->number) {
      e->returnTypes = {ast::ObjectValueContainer(e->number->type)};
    } else if (e->string) {
      static ast::ObjectValueContainer stringType = a_database->getType("STRING", "STANDARD", "STD");
      e->returnTypes = {stringType};
    } else if (e->identifier) {
      e->returnTypes = basicIdentifierReturnTypes(e->identifier);
    } else if (e->character) {
      std::string n = e->character->toString();
      auto valid = [&](DatabaseElement* e) {
        return true;
      };
      e->returnTypes = getReturnTypes(n, valid);
    } else {
      exceptions.printInternal("Unknown expression term", e->text);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression term", e->text);
    }
    debug.functionEnd("expressionTermReturnTypes");
    return e->returnTypes;
  }

  ast::ObjectArguments ExpressionParser::toObjectArguments(ast::AssociationList* associationList) {
    debug.functionStart("toObjectArguments");
    ast::ObjectArguments result(false);
    if (associationList) {
      for (auto& i : associationList->associationElements.list) {
        ast::ObjectArgument x;
        x.name = (i.formalPart && i.formalPart->name) ? i.formalPart->name->toString(true) : "";
        ReturnTypes r = expressionReturnTypes(i.actualPart);
        removeDuplicates(r);
        if (r.size() == 1) {
          x.type = *(r.begin());
          result.push_back(x);
        } else {
          ast::Text* text = i.formalPart ? &i.formalPart->name->text : NULL;
          if (r.empty()) {
            exceptions.printError("Could not resolve argument type", text);
          } else {
            std::string found = returnTypesToString(r);
            exceptions.printError("More than one type for argument. Found the following types " + found, text);
          }
        }
      }
    }
    debug.functionEnd("toObjectArguments");
    return result;
  }
 
  
  ExpressionParser::ReturnTypes ExpressionParser::attributeReturnTypes(std::string& name, std::string& attribute,
                                                                       ast::AssociationList* associationList) {
    debug.functionStart("attributeReturnTypes");
    ReturnTypes result;
    // ast::ObjectArguments arguments = toObjectArguments(associationList);
    DatabaseResult object;
    if (a_database->findOne(object, name)) {
      ast::ObjectValueContainer type = getAttributeType(object.object->type, attribute);
      result.push_back(type);
    } else {
      exceptions.printError("Could not resolve type of attribute \"" + attribute + "\" of \"" + name + "\"");
    }
    debug.functionEnd("attributeReturnTypes");
    return result;
  }

  ExpressionParser::ReturnTypes ExpressionParser::basicIdentifierReturnTypes(ast::BasicIdentifier* b) {
    debug.functionStart("basicIdentifierReturnTypes");
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
      a_database->printAllObjects(name);
    }
    debug.functionEnd("basicIdentifierReturnTypes");
    return b->returnTypes;
  }

  bool ExpressionParser::getStaticAttributeType(std::string attributeName, ast::ObjectValueContainer& result) {
    static ast::ObjectValueContainer stringType = a_database->getType("STRING", "STANDARD", "STD");
    static std::unordered_map<std::string, ast::ObjectValueContainer> fixedAttributeTypes =
      {{"IMAGE", stringType},
       {"LENGTH", ast::ObjectValueContainer(ast::ObjectValue::INTEGER)}};
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
    ast::ObjectValueContainer result(ast::ObjectValue::UNKNOWN);
    if (!getStaticAttributeType(attributeName, result)) {
      if (attributeName == "HIGH" || attributeName == "LOW" || attributeName == "LEFT" || attributeName == "RIGHT") {
        switch(type.value) {
        case ast::ObjectValue::INTEGER: result = ast::ObjectValueContainer(ast::ObjectValue::INTEGER); break;
        case ast::ObjectValue::PHYSICAL: result = ast::ObjectValueContainer(ast::ObjectValue::PHYSICAL); break;
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

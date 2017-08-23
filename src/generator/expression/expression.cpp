#include <algorithm>

#include "expression.hpp"

namespace generator {

  std::string ExpressionParser::ReturnTypesToString(const ast::ReturnTypes& return_types) {
    debug.functionStart("ReturnTypesToString", true);
    std::string found = "";
    std::string delimiter;
    for (auto& i : return_types) {
      found += delimiter + i.toString();
      delimiter = ", ";
    }
    debug.debug("Result = " + found, true);
    debug.functionEnd("ReturnTypesToString");
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

  bool ExpressionParser::UniqueReturnType(ast::ReturnTypes& return_types, ast::ObjectValueContainer& type, ast::Text* text) {
    bool result = true;
    if (return_types.size() == 1) {
      type = *return_types.begin();
    } else {
      result = false;
      std::string found = ReturnTypesToString(return_types);
      exceptions.printError("Could not resolve unique type. Found the following types: " + found, text);
    }
    return result;
  }
  
  bool ExpressionParser::CollectAllReturnTypes(ast::Expression* e,
                                               ast::ObjectValueContainer& expectedType) {
    debug.functionStart("CollectAllReturnTypes");
    bool ok = false;
    ast::ReturnTypes o;
    ExpressionReturnTypes(e, o);
    if (o.find(expectedType) != o.end()) {
      ok = true;
    } else {
      std::string found = ReturnTypesToString(o);
      exceptions.printError("Could not resolve expected type " + expectedType.toString() +
                            ". Found the following types: " + found, e->text);
    }
    debug.functionEnd("CollectAllReturnTypes");
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

  bool ExpressionParser::objectWithArguments(DatabaseElement* e, ast::ObjectArguments& arguments,
                                             ast::ObjectValueContainer* expectedReturnType) {
    debug.functionStart("objectWithArguments(e = " + e->toString() + ", arguments = " + arguments.toString() + ")");
    bool result;
    if (e->id == ast::ObjectType::FUNCTION && !e->arguments.equals(arguments, debug.IsVerbose())) {
      result = false;
    } else if (e->type.GetValue() == ast::ObjectValue::ARRAY && !arguments.equals(e->type.GetArguments(), true, debug.IsVerbose())) {
      result = false;
    } else {
      if (expectedReturnType) {
        ast::ObjectValueContainer* type;
        if (e->type.GetValue() == ast::ObjectValue::ARRAY) {
          ast::ObjectValueContainer::Array& subtype = e->type.GetSubtype();
          assert(subtype.size() == 1);
          type = &subtype.front();
        } else {
          type = &e->type; 
        }
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
  
  void ExpressionParser::FunctionReturnTypes(std::string& name,
                                             ast::AssociationList* associationList,
                                             ast::ReturnTypes& return_types) {
    debug.functionStart("functionReturnTypes");
    assert(associationList);
    ast::ObjectArguments arguments = toObjectArguments(associationList);
    auto valid = [&](DatabaseElement* e) {
      return objectWithArguments(e, arguments);
    };
    DatabaseResults objects;
    a_database->findAll(objects, name);
    for (auto& i : objects) {
      ast::ObjectValueContainer::Array& subtype = i.object->type.GetSubtype();
      assert(subtype.size() < 2);
      if (!subtype.empty()) {
        return_types.insert(subtype.front());
      } else {
        return_types.insert(i.object->type);
      }
    }
    debug.functionEnd("FunctionReturnTypes");
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
  
  void ExpressionParser::GetStandardOperatorReturnTypes(const std::string& name,
                                                        const ast::ObjectValueContainer& l,
                                                        const ast::ObjectValueContainer& r,
                                                        ast::ReturnTypes& return_types) {
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
    if (l.GetValue() == r.GetValue()) {
      auto i = translate.find(name);
      if (i != translate.end()) {
	Map& m = i->second;
	if ((m.l == DONT_CARE || m.l == l.GetValue()) &&
	    (m.r == DONT_CARE || m.r == r.GetValue()) &&
            l.equals(r)) {
	  ast::ObjectValueContainer v;
	  if (m.result == DONT_CARE || m.result == m.l) {
	    v = l;
	  } else {
	    v = ast::ObjectValueContainer(m.result);
	  }
	  return_types.insert(v);
	}
      }
    }
  }
  
  void ExpressionParser::OperatorReturnTypes(const std::string& name,
                                             const ast::ObjectValueContainer& l,
                                             const ast::ObjectValueContainer& r,
                                             ast::ReturnTypes& return_types) {
    debug.functionStart("OperatorReturnTypes");
    std::list<ast::ObjectArgument> a = {ast::ObjectArgument(l), ast::ObjectArgument(r)};
    auto x = ast::ObjectArguments(false, a);
    auto valid = [&](DatabaseElement* e) {
      return x.equals(e->arguments);
    };
    GetReturnTypes(name, valid, return_types);
    GetStandardOperatorReturnTypes(name, l, r, return_types);
    debug.functionEnd("OperatorReturnTypes");
  }

  void ExpressionParser::ExpressionReturnTypes(ast::Expression* e, ast::ReturnTypes& return_types) {
    debug.functionStart("ExpressionReturnTypes");
    assert(e);
    if (e->unaryOperator) {
      ExpressionReturnTypes(e->expression, e->returnTypes);
    } else if (e->op) {
      ast::ReturnTypes term; 
      ast::ReturnTypes expression; 
      ExpressionTermReturnTypes(e->term, term);
      ExpressionReturnTypes(e->expression, expression);
      for (auto& i : term) {
        for (auto& j : expression) {
          OperatorReturnTypes(e->op->op, i, j, e->returnTypes);
        }
      }
      if (e->returnTypes.empty()) {
        exceptions.printError("Could not resolve type of expression \"" +
                              ReturnTypesToString(term) + "\" " + e->op->op + " \"" +
                              ReturnTypesToString(expression) + "\"", e->text);
      }
    } else {
      ExpressionTermReturnTypes(e->term, e->returnTypes);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression", e->text);
    }
    return_types.insert(e->returnTypes.begin(), e->returnTypes.end());
    debug.functionEnd("expressionReturnTypes");
  }

  std::string ExpressionParser::physicalToString(ast::Physical* physical) {
    debug.functionStart("physicalToString");
    std::string enumName = a_name_converter->GetName(physical->unit, ast::ObjectType::ENUM);
    std::string result = "{" + physical->number->toString() + ", " + enumName + "}";
    debug.functionEnd("physicalToString");
    return result;
  }
  
  void ExpressionParser::ExpressionTermReturnTypes(ast::ExpressionTerm* e, ast::ReturnTypes& return_types) {
    debug.functionStart("ExpressionTermReturnTypes");
    assert(e);
    if (!e->parenthis.list.empty()) {
      if (e->parenthis.list.size() > 1) {
        ast::Expression& x = e->parenthis.list.back();
        ast::ReturnTypes return_types;
        ExpressionReturnTypes(&x, return_types);
        for (auto& i : return_types) {
          ast::ObjectValueContainer a(ast::ObjectValue::ARRAY, i);
          e->returnTypes.insert(a);
        }
      } else {
        ast::Expression& x = e->parenthis.list.back();
        ExpressionReturnTypes(&x, e->returnTypes);
      }
    } else if (e->physical) {
      e->returnTypes.insert(ast::ObjectValueContainer(ast::ObjectValue::PHYSICAL, e->physical->unit->toString(true)));
    } else if (e->number) {
      e->returnTypes.insert(ast::ObjectValueContainer(e->number->type));
    } else if (e->string) {
      static ast::ObjectValueContainer enum_type(ast::ObjectValue::ENUMERATION);
      static ast::ObjectValueContainer string_type(ast::ObjectValue::ARRAY, enum_type);
      e->returnTypes.insert(string_type);
    } else if (e->identifier) {
      BasicIdentifierReturnTypes(e->identifier, e->returnTypes);
    } else if (e->character) {
      std::string n = e->character->toString();
      auto valid = [&](DatabaseElement* e) {
        return true;
      };
      GetReturnTypes(n, valid, e->returnTypes);
    } else {
      exceptions.printInternal("Unknown expression term", e->text);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression term", e->text);
    }
    return_types.insert(e->returnTypes.begin(), e->returnTypes.end());
    debug.functionEnd("ExpressionTermReturnTypes");
  }

  ast::ObjectArguments ExpressionParser::toObjectArguments(ast::AssociationList* associationList) {
    debug.functionStart("toObjectArguments");
    ast::ObjectArguments result(false);
    if (associationList) {
      for (auto& i : associationList->associationElements.list) {
        ast::ObjectArgument x;
        x.name = (i.formalPart && i.formalPart->name) ? i.formalPart->name->toString(true) : "";
        ast::ReturnTypes r;
        ExpressionReturnTypes(i.actualPart, r);
        if (r.size() == 1) {
          x.type = *(r.begin());
          result.push_back(x);
        } else {
          ast::Text* text = i.formalPart ? &i.formalPart->name->text : NULL;
          if (r.empty()) {
            exceptions.printError("Could not resolve argument type", text);
          } else {
            std::string found = ReturnTypesToString(r);
            exceptions.printError("More than one type for argument. Found the following types " + found, text);
          }
        }
      }
    }
    debug.functionEnd("toObjectArguments");
    return result;
  }
 
  
  void ExpressionParser::AttributeReturnTypes(std::string& name, std::string& attribute,
                                              ast::AssociationList* associationList,
                                              ast::ReturnTypes& return_types) {
    debug.functionStart("AttributeReturnTypes");
    // ast::ObjectArguments arguments = toObjectArguments(associationList);
    DatabaseResult object;
    if (a_database->findOne(object, name)) {
      ast::ObjectValueContainer type = getAttributeType(object.object->type, attribute);
      return_types.insert(type);
    } else {
      exceptions.printError("Could not resolve type of attribute \"" + attribute + "\" of \"" + name + "\"");
    }
    debug.functionEnd("AttributeReturnTypes");
  }

  void ExpressionParser::BasicIdentifierReturnTypes(ast::BasicIdentifier* b,
                                                    ast::ReturnTypes& return_types) {
    debug.functionStart("BasicIdentifierReturnTypes");
    std::string name = b->text.toString(true);
    if (b->attribute) {
      std::string attribute = b->attribute->toString(true);
      AttributeReturnTypes(name, attribute, b->arguments, b->returnTypes);
    } else if (b->arguments) {
      FunctionReturnTypes(name, b->arguments, b->returnTypes); 
    } else {
      auto valid = [&](DatabaseElement* e) {
        return e->arguments.empty();
      };
      GetReturnTypes(name, valid, b->returnTypes);
    }
    if (b->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of " + name, &b->text);
      a_database->printAllObjects(name);
    }
    return_types.insert(b->returnTypes.begin(), b->returnTypes.end());
    debug.functionEnd("BasicIdentifierReturnTypes");
  }

  bool ExpressionParser::getStaticAttributeType(std::string attributeName, ast::ObjectValueContainer& result) {
    static ast::ObjectValueContainer enum_type =  ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION);
    static ast::ObjectValueContainer string_type = ast::ObjectValueContainer(ast::ObjectValue::ARRAY, enum_type); 
    static std::unordered_map<std::string, ast::ObjectValueContainer> fixedAttributeTypes =
      {{"IMAGE", string_type},
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
        switch(type.GetValue()) {
        case ast::ObjectValue::INTEGER: 
        case ast::ObjectValue::PHYSICAL: 
        case ast::ObjectValue::ENUMERATION: result = type; break;
        case ast::ObjectValue::ARRAY: {
          ast::ObjectValueContainer::Array& subtype = type.GetSubtype();
          assert(subtype.size() == 1);
          result = subtype.front();
          break;
        }
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
  
  std::string ExpressionParser::BasicIdentifierToString(ast::BasicIdentifier* identifier,
                                                        ast::ObjectValueContainer* expected_type) {
    return BasicIdentifierToString(identifier, expected_type, [](DatabaseResult& r) {});
  }
      

}

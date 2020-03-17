#include <algorithm>

#include "expression.hpp"

namespace generator {

  std::string ExpressionParser::returnTypesToString(parameters& parm,
						    ast::ReturnTypes& return_types) {
    m_debug.functionStart("ReturnTypesToString");
    std::string found = "";
    std::string delimiter;
    for (auto& i : return_types) {
      found += delimiter + i.toString();
      delimiter = ", ";
    }
    m_debug.debug("Result = " + found, true);
    m_debug.functionEnd("ReturnTypesToString");
    return found;
  }

  bool ExpressionParser::exists(parameters& parm,
				ReturnTypePair& pair,
				std::list<ReturnTypePair>& typePairs) {
    for (auto& i : typePairs) {
      if ((i.left.equals(pair.left) && i.right.equals(pair.right)) ||
          (i.left.equals(pair.right) && i.right.equals(pair.left))) {
        return true;
      }
    }
    return false;
  }

  bool ExpressionParser::uniqueReturnType(parameters& parm,
					  ast::ReturnTypes& return_types,
					  ast::ObjectValueContainer& type,
					  ast::Text* text) {
    bool result = true;
    if (return_types.size() == 1) {
      type = *return_types.begin();
    } else {
      result = false;
      std::string found = returnTypesToString(parm, return_types);
      exceptions.printError("Could not resolve unique type. Found the following types: " + found, text);
    }
    return result;
  }
  
  ast::ObjectValueContainer ExpressionParser::collectAllReturnTypes(parameters& parm,
								    ast::Expression* e,
								    ast::ObjectValueContainer& expectedType) {
    m_debug.functionStart("CollectAllReturnTypes");
    ast::ObjectValueContainer result;
    ast::ReturnTypes o;
    expressionReturnTypes(parm, e, o);
    auto found_type = o.find(expectedType);
    if (found_type != o.end()) {
      result = *found_type;
    } else {
      std::string found = returnTypesToString(parm, o);
      exceptions.printError("Could not resolve expected type " + expectedType.toString() +
                            ". Found the following types: " + found, e->text);
    }
    m_debug.functionEnd("CollectAllReturnTypes");
    return result;
  }

  bool ExpressionParser::collectUniqueReturnType(parameters& parm,
						 ast::Expression* e,
                                                 ast::ObjectValueContainer& type) {
    ast::ReturnTypes return_types;
    expressionReturnTypes(parm, e, return_types);   
    return uniqueReturnType(parm, return_types, type, e->text);
  }
  
  std::string ExpressionParser::toString(parameters& parm,
					 ast::Expression* e,
					 ast::ObjectValueContainer& expectedType,
					 bool add_read_function) {
    m_debug.functionStart("toString");
    auto sensitivityListCallback =
      [&](DatabaseResult& object, std::string& name_extension) {
	if (add_read_function) {
	  name_extension = ".read()";
	}
      };
    std::string s = assignmentString(parm, e, expectedType, sensitivityListCallback);
    m_debug.functionEnd("toString: s = " + s);
    return s;
  }

  std::string ExpressionParser::toString(parameters& parm,
					 ast::Expression* expr,
					 ast::ObjectValue expectedType) {
    m_debug.functionStart("toString");
    ast::ObjectValueContainer e(expectedType);
    std::string s = toString(parm, expr, e);
    m_debug.functionEnd("toString");
    return s;
  }


  std::string ExpressionParser::procedureCallStatementToString(parameters& parm,
							       ast::ProcedureCallStatement* p) {
    assert(p);
    std::string name = p->name->toString(true);
    m_debug.functionStart("procedureCallStatementToString(name = " + name + ")");
    ast::ObjectArguments arguments = toObjectArguments(parm, p->arguments);
    auto valid = [&](DatabaseElement* e) {
      bool result = ((e->id == ast::ObjectType::PROCEDURE) &&
                     e->arguments.equals(arguments, m_debug.isVerbose()));
      m_debug.debug(e->toString() + (result ? " == " : " != ") + name + "(" + arguments.toString() + ")",
                  true, (result ? Output::Color::GREEN : Output::Color::RED));
      return result;
    };
    DatabaseResult object;
    if (parm.findOneBase(object, name, valid)) {
      bool double_brackets = false;
      name = objectToString(parm, object, p->arguments, [&](DatabaseResult& e, std::string& name_extension) {}, double_brackets);
    } else {
      exceptions.printError("Could not find definition of procedure \"" + name + "\"", &p->name->text);
      parm.printAllObjects(name);
    }
    m_debug.functionEnd("procedureCallStatementToString");
    return name;
  }

  bool ExpressionParser::objectWithArguments(parameters& parm,
					     DatabaseElement* e,
					     ast::ObjectArguments& arguments,
                                             ast::ObjectValueContainer* expectedReturnType) {
    m_debug.functionStart("objectWithArguments(e = " + e->toString() + ", arguments = " + arguments.toString() + ")");
    bool result;
    bool cast_operation = (e->id == ast::ObjectType::TYPE);
    if (e->id == ast::ObjectType::FUNCTION && !e->arguments.equals(arguments, false, m_debug.isVerbose())) {
      result = false;
    } else if (cast_operation) {
      result = arguments.equals(e->type, m_debug.isVerbose());
    } else if (e->type.GetValue() == ast::ObjectValue::ARRAY && !arguments.equals(e->type.GetArguments(), true, m_debug.isVerbose())) {
      result = false;
    } else {
      if (expectedReturnType) {
        ast::ObjectValueContainer* type;
        if (e->type.GetValue() == ast::ObjectValue::ARRAY && !arguments.empty()) {
          ast::ObjectValueContainer::Array& subtype = e->type.GetSubtype();
          assert(subtype.size() == 1);
          type = &subtype.front();
        } else {
          type = &e->type; 
        }
        result = type->equals(*expectedReturnType); 
        m_debug.debug("Return type " + type->toString() + (result ? " == " : " != ") +
                    expectedReturnType->toString());
      } else {
        result = true;
      }
    }
    m_debug.functionEnd("objectWithArguments = " + std::string(result ? "true" : "false"));
    return result;
  }
  
  void ExpressionParser::functionReturnTypes(parameters& parm, std::string& name,
                                             ast::AssociationList* associationList,
                                             ast::ReturnTypes& return_types) {
    m_debug.functionStart("functionReturnTypes(name = " + name + ")");
    assert(associationList);
    ast::ObjectArguments arguments = toObjectArguments(parm, associationList);
    /* auto valid =
      [&](DatabaseElement* e) {
	return objectWithArguments(parm, e, arguments);
	}; */
    DatabaseResults objects;
    parm.findAll(objects, name);
    for (auto& i : objects) {
      m_debug.debug("Found object: " + i.toString());
      bool cast_operation = (i.object->id == ast::ObjectType::TYPE);
      if (cast_operation) {
        return_types.insert(i.object->type);
      } else {
        ast::ObjectValueContainer::Array& subtype = i.object->type.GetSubtype();
        assert(subtype.size() < 2);
        if (!subtype.empty()) {
          return_types.insert(subtype.front());
        } else {
          return_types.insert(i.object->type);
        }
      }
    }
    m_debug.functionEnd("FunctionReturnTypes = " + returnTypesToString(parm, return_types));
  }

  std::string ExpressionParser::translateOperator(parameters& parm,
						  std::string& op) {
    static std::unordered_map<std::string, std::string> translate =
      { {"/=", "!="}, {"=", "=="}, {">=", ">="}, {"+", "+"}, {"-", "-"}, {"&", "+"}, {"**", "^"} };
    auto i = translate.find(op);
    std::string result = op;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    result[0] = toupper(result[0]);
    if (i != translate.end()) {
      result  = i->second;
    }
    return result;
  }

  void ExpressionParser::getStandardOperatorReturnTypes(parameters& parm,
							std::string& name,
                                                        const ast::ObjectValueContainer& l,
                                                        const ast::ObjectValueContainer& r,
                                                        ast::ReturnTypes& return_types) {
    m_debug.functionStart("GetStandardOperatorReturnTypes(name = " + name + ", l = " + l.toString() + ", r = " + r.toString() + ")");
    struct OperatorReturnMap {
      ast::ObjectValue l;
      ast::ObjectValue r;
      ast::ObjectValue result;};
    const static ast::ObjectValue BOOLEAN = ast::ObjectValue::BOOLEAN;
    const static ast::ObjectValue ARRAY = ast::ObjectValue::ARRAY;
    const static ast::ObjectValue DONT_CARE = ast::ObjectValue::DONT_CARE;
    static std::unordered_map<std::string, OperatorReturnMap> translate =
      {
        {"&", {ARRAY, ARRAY, ARRAY}},
	{"+", {DONT_CARE, DONT_CARE, DONT_CARE}},
	{"-", {DONT_CARE, DONT_CARE, DONT_CARE}},
	{"=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{"/=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{"<=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{">=", {DONT_CARE, DONT_CARE, BOOLEAN}},
	{"and", {BOOLEAN, BOOLEAN, BOOLEAN}},
	{"or", {BOOLEAN, BOOLEAN, BOOLEAN}}
      };
    if (l.GetValue() == r.GetValue()) {
      auto i = translate.find(name);
      if (i != translate.end()) {
        OperatorReturnMap& map = i->second;
        ast::ObjectValueContainer lv(map.l);
        ast::ObjectValueContainer rv(map.r);
        if ((map.l == DONT_CARE || lv.equals(l)) &&
            (map.r == DONT_CARE || rv.equals(r)) &&
            l.equals(r)) {
          ast::ObjectValueContainer v;
          if (map.result == DONT_CARE || map.result == map.l) {
            v = l;
          } else {
            v = ast::ObjectValueContainer(map.result);
          }
          return_types.insert(v);
        }
      }
    }
    m_debug.functionEnd("GetStandardOperatorReturnTypes = " + returnTypesToString(parm, return_types));
  }
  
  void ExpressionParser::operatorReturnTypes(parameters& parm,
					     std::string& name,
                                             const ast::ObjectValueContainer& l,
                                             const ast::ObjectValueContainer& r,
                                             ast::ReturnTypes& return_types) {
    m_debug.functionStart("OperatorReturnTypes");
    std::list<ast::ObjectArgument> a = {ast::ObjectArgument(l), ast::ObjectArgument(r)};
    auto x = ast::ObjectArguments(false, a);
    auto valid = [&](DatabaseElement* e) {
      return x.equals(e->arguments);
    };
    getReturnTypes(parm, name, valid, return_types);
    getStandardOperatorReturnTypes(parm, name, l, r, return_types);
    m_debug.functionEnd("OperatorReturnTypes");
  }

  void ExpressionParser::expressionReturnTypes(parameters& parm, ast::Expression* e, ast::ReturnTypes& return_types) {
    m_debug.functionStart("ExpressionReturnTypes");
    assert(e);
    if (e->unaryOperator) {
      expressionReturnTypes(parm, e->expression, e->returnTypes);
    } else if (e->op) {
      ast::ReturnTypes term; 
      ast::ReturnTypes expression; 
      expressionTermReturnTypes(parm, e->term, term);
      expressionReturnTypes(parm, e->expression, expression);
      for (auto& i : term) {
        for (auto& j : expression) {
          operatorReturnTypes(parm, e->op->op, i, j, e->returnTypes);
        }
      }
      if (e->returnTypes.empty()) {
        exceptions.printError("Could not resolve type of expression \"" +
                              returnTypesToString(parm, term) + "\" " + e->op->op + " \"" +
                              returnTypesToString(parm, expression) + "\"", e->text);
      }
    } else {
      expressionTermReturnTypes(parm, e->term, e->returnTypes);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression", e->text);
    }
    return_types.insert(e->returnTypes.begin(), e->returnTypes.end());
    m_debug.functionEnd("expressionReturnTypes = " + returnTypesToString(parm, return_types));
  }

  std::string ExpressionParser::physicalToString(parameters& parm,
						 ast::Physical* physical) {
    m_debug.functionStart("physicalToString");
    std::string enumName = parm.getName(physical->unit, ast::ObjectType::ENUM);
    std::string result = "{" + physical->number->toString() + ", " + enumName + "}";
    m_debug.functionEnd("physicalToString");
    return result;
  }

  void ExpressionParser::parenthesisReturnTypes(parameters& parm,
						ast::ExpressionTerm* e) {
    m_debug.functionStart("ParenthesisReturnTypes");
    m_debug.debug("parenthis size = " + std::to_string(e->parenthis.list.size()));
    ast::ElementAssociation& x = e->parenthis.list.back();
    ast::Expression* expression = x.expression;
    if (expression == NULL) {
      assert(x.choises);
      if (x.choises->choises.list.size() == 1) {
        expression = x.choises->choises.list.back().expression;
      } else {
        exceptions.printInternal("Could not resolve type of more than one choise", e->text);
      }
    }
    bool choise_exists = (x.expression ? true : false);
    if (e->parenthis.list.size() > 1 || choise_exists) {
      ast::ReturnTypes return_types;
      expressionReturnTypes(parm, expression, return_types);
      for (auto& i : return_types) {
        ast::ObjectValueContainer a(ast::ObjectValue::ARRAY, i);
        e->returnTypes.insert(a);
        if (i.IsValue(ast::ObjectValue::ARRAY)) {
          ast::ObjectValueContainer::Array arguments;
          ast::ObjectValueContainer unknown_argument;
          arguments.push_back(unknown_argument);
          arguments.push_back(unknown_argument);
          ast::ObjectValueContainer subtype;
          i.SetSubtype(subtype);
          ast::ObjectValueContainer a(ast::ObjectValue::ARRAY, arguments, subtype);
          e->returnTypes.insert(a);
        }
      }
    } else {
      expressionReturnTypes(parm, expression, e->returnTypes);
    }
    m_debug.functionEnd("ParenthesisReturnTypes");
  }
  
  void ExpressionParser::expressionTermReturnTypes(parameters& parm, ast::ExpressionTerm* e, ast::ReturnTypes& return_types) {
    m_debug.functionStart("ExpressionTermReturnTypes");
    assert(e);
    if (!e->parenthis.list.empty()) {
      parenthesisReturnTypes(parm, e);
    } else if (e->physical) {
      e->returnTypes.insert(ast::ObjectValueContainer(ast::ObjectValue::PHYSICAL, e->physical->unit->toString(true)));
    } else if (e->number) {
      e->returnTypes.insert(ast::ObjectValueContainer(e->number->type));
    } else if (e->string) {
      static ast::ObjectValueContainer enum_type(ast::ObjectValue::ENUMERATION);
      static ast::ObjectValueContainer string_type(ast::ObjectValue::ARRAY, enum_type);
      e->returnTypes.insert(string_type);
    } else if (e->identifier) {
      basicIdentifierReturnTypes(parm, e->identifier, e->returnTypes);
    } else if (e->character) {
      std::string n = e->character->toString();
      auto valid = [&](DatabaseElement* e) {
        return true;
      };
      getReturnTypes(parm, n, valid, e->returnTypes);
    } else {
      exceptions.printInternal("Unknown expression term", e->text);
    }
    if (e->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of expression term", e->text);
    }
    return_types.insert(e->returnTypes.begin(), e->returnTypes.end());
    m_debug.functionEnd("ExpressionTermReturnTypes");
  }

  ast::ObjectArguments ExpressionParser::toObjectArguments(parameters& parm,
							   ast::AssociationList* associationList) {
    m_debug.functionStart("toObjectArguments", false, __FILE__, __LINE__);
    ast::ObjectArguments result(false);
    if (associationList) {
      for (auto& i : associationList->associationElements.list) {
        ast::ObjectArgument x;
        x.name = (i.formalPart && i.formalPart->name) ? i.formalPart->name->toString(true) : "";
        ast::ReturnTypes r;
        expressionReturnTypes(parm, i.actualPart, r);
        if (r.size() == 1) {
          x.type = *(r.begin());
          result.push_back(x);
        } else {
          ast::Text* text = i.formalPart ? &i.formalPart->name->text : NULL;
          if (r.empty()) {
            exceptions.printError("Could not resolve argument type", text);
          } else {
            std::string found = returnTypesToString(parm, r);
            exceptions.printError("More than one type for argument. Found the following types " + found, text);
          }
        }
      }
    }
    m_debug.functionEnd("toObjectArguments = " + result.toString());
    return result;
  }
 
  
  void ExpressionParser::attributeReturnTypes(parameters& parm, std::string& name, std::string& attribute,
                                              ast::AssociationList* associationList,
                                              ast::ReturnTypes& return_types) {
    m_debug.functionStart("AttributeReturnTypes");
    // ast::ObjectArguments arguments = toObjectArguments(associationList);
    DatabaseResult object;
    if (parm.findOne(object, name)) {
      ast::ObjectValueContainer type = getAttributeType(parm, object.object->type, attribute);
      return_types.insert(type);
    } else {
      exceptions.printError("Could not resolve type of attribute \"" + attribute + "\" of \"" + name + "\"");
    }
    m_debug.functionEnd("AttributeReturnTypes");
  }

  void ExpressionParser::basicIdentifierReturnTypes(parameters& parm,
						    ast::BasicIdentifier* b,
                                                    ast::ReturnTypes& return_types) {
    std::string name = b->toString(true);
    m_debug.functionStart("BasicIdentifierReturnTypes(name = " + name + ")", true);
    if (b->attribute) {
      std::string attribute = b->attribute->toString(true);
      attributeReturnTypes(parm, name, attribute, b->arguments, b->returnTypes);
    } else if (b->arguments) {
      functionReturnTypes(parm, name, b->arguments, b->returnTypes); 
    } else {
      auto valid = [&](DatabaseElement* e) {
        return e->arguments.empty();
      };
      getReturnTypes(parm, name, valid, b->returnTypes);
    }
    if (b->returnTypes.empty()) {
      exceptions.printError("Could not resolve type of " + name, &b->getText());
    }
    return_types.insert(b->returnTypes.begin(), b->returnTypes.end());
    m_debug.functionEnd("BasicIdentifierReturnTypes = " + returnTypesToString(parm, return_types));
  }

  bool ExpressionParser::getStaticAttributeType(parameters& parm,
						std::string attributeName,
						ast::ObjectValueContainer& result) {
    static ast::ObjectValueContainer enum_type =  ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION);
    static ast::ObjectValueContainer string_type = ast::ObjectValueContainer(ast::ObjectValue::ARRAY, enum_type); 
    static std::unordered_map<std::string, ast::ObjectValueContainer> fixedAttributeTypes =
      {{"IMAGE", string_type},
       {"LENGTH", ast::ObjectValueContainer(ast::ObjectValue::INTEGER)},
       {"POS", ast::ObjectValueContainer(ast::ObjectValue::INTEGER)},
       {"EVENT", ast::ObjectValueContainer(ast::ObjectValue::BOOLEAN)}};
    bool found = false;
    auto i = fixedAttributeTypes.find(attributeName);
    if (i != fixedAttributeTypes.end()) {
      result = i->second;
      found = true;
    }
    return found;
  }
  
  ast::ObjectValueContainer ExpressionParser::getAttributeType(parameters& parm,
							       ast::ObjectValueContainer& type,
                                                               std::string attributeName) {
    m_debug.functionStart("getAttributeType(attributeName = " + attributeName + ")");
    ast::ObjectValueContainer result(ast::ObjectValue::UNKNOWN);
    if (!getStaticAttributeType(parm, attributeName, result)) {
      if (attributeName == "HIGH" || attributeName == "LOW" || attributeName == "LEFT" ||
	  attributeName == "RIGHT" || attributeName == "LAST_VALUE") {
        switch(type.GetValue()) {
        case ast::ObjectValue::INTEGER: 
        case ast::ObjectValue::PHYSICAL: 
        case ast::ObjectValue::ENUMERATION: result = type; break;
        case ast::ObjectValue::ARRAY: result = type.GetArgument(); break;
        default: exceptions.printError("Could not find attribute \"" + attributeName + "\" of type " + type.toString(), __FILE__, __LINE__); 
        };
      } else {
        exceptions.printError("Could not resolve type of attribute " + attributeName + " with type " + type.toString(), __FILE__, __LINE__);
      }
    }
    m_debug.functionEnd("getAttributeType = " + result.toString());
    return result;
  }
  
  bool ExpressionParser::findAttributeMatch(parameters& parm,
					    DatabaseResults& objects,
                                            DatabaseResult& match,
                                            ast::ObjectValueContainer& expectedType,
                                            std::string& name) {
    m_debug.functionStart("findAttributeMatch(name = " + name + ")");
    bool foundMatch = false;;
    m_debug.debug("objects.empty() = " + std::to_string(objects.empty()));
    for (auto& i : objects) {
      ast::ObjectValueContainer a = getAttributeType(parm, i.object->type, name);
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
    m_debug.functionEnd("findAttributeMatch : " + std::to_string(foundMatch));
    return foundMatch;
  }
  
  std::string ExpressionParser::basicIdentifierToString(parameters& parm,
							ast::BasicIdentifier* identifier,
                                                        ast::ObjectValueContainer* expected_type) {
    return basicIdentifierToString(parm, identifier, expected_type, [](DatabaseResult& r, std::string& name_extension) {});
  }
      

}

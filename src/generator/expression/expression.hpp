#ifndef _GENERATOR_EXPRESSION_PARSER_HPP
#define _GENERATOR_EXPRESSION_PARSER_HPP

#include <list>
#include <cassert>

#include "../../ast/object_type.hpp"
#include "../../exceptions/exceptions.hpp"
#include "../../debug/debug.hpp"
#include "../database/database.hpp"
#include "../name_converter.hpp"

namespace generator {

  class ExpressionParser {

    Debug<false> debug;
    
    bool verbose = false;
    Database* a_database;
    NameConverter* a_name_converter;
    Exceptions exceptions;

    struct ReturnTypePair {
      ast::ObjectValueContainer left;
      ast::ObjectValueContainer right;
    };

    // Exception classes
  public:
    
    class ObjectNotFound {
      ExpressionParser* a_parent;
      std::string a_name;
      std::string a_message;
      ast::Text* a_text;
    public:
      ObjectNotFound(ExpressionParser* parent, std::string name,
                     std::string message, ast::Text* text) {
        a_parent = parent;
        a_name = name;
        a_message = message;
        a_text = text;
      }
      void print() {
        a_parent->exceptions.printError(a_message, a_text);
	a_parent->a_database->printAllObjects(a_name);
      }
    };

  private:
    template<typename Func>
    void GetReturnTypes(const std::string& name, Func valid, ast::ReturnTypes& return_types); 
    void GetStandardOperatorReturnTypes(const std::string& name,
                                        const ast::ObjectValueContainer& l,
                                        const ast::ObjectValueContainer& r,
                                        ast::ReturnTypes& return_types);
   
    // First-pass functions
    void ExpressionReturnTypes(ast::Expression* e, ast::ReturnTypes& return_types);   
    void ExpressionTermReturnTypes(ast::ExpressionTerm* e, ast::ReturnTypes& return_types);  
  public:
    void BasicIdentifierReturnTypes(ast::BasicIdentifier* b, ast::ReturnTypes& return_types);
  private:
    void OperatorReturnTypes(const std::string& name,
                             const ast::ObjectValueContainer& l,
                             const ast::ObjectValueContainer& r,
                             ast::ReturnTypes& return_types);
    void FunctionReturnTypes(std::string& name, ast::AssociationList* associationList,
                             ast::ReturnTypes& return_types);
    void AttributeReturnTypes(std::string& name, std::string& attribute,
                              ast::AssociationList* associationList, ast::ReturnTypes& return_types);
    
    // Second-pass functions 
    template <typename Func>
    std::string operationToString(ast::Expression* e,
                                  ast::ObjectValueContainer& expectedType,
                                  Func sensitivityListCallback);
    template <typename Func>
    std::string expressionToString(ast::Expression* e,
                                   ast::ObjectValueContainer& expectedType,
                                   Func sensitivityListCallback,
                                   const bool double_brackets);
    template <typename Func>
    std::string expressionTermToString(ast::ExpressionTerm* e,
                                       ast::ObjectValueContainer& expectedType,
                                       Func sensitivityListCallback,
                                       const bool double_brackets);
  public:
    template <typename Func>
    std::string BasicIdentifierToString(ast::BasicIdentifier* identifier,
                                        ast::ObjectValueContainer* expectedType,
                                        Func sensitivityListCallback);
    std::string BasicIdentifierToString(ast::BasicIdentifier* identifier,
                                        ast::ObjectValueContainer* expectedType = NULL);
  private:
    template <typename Func>
    std::string objectToString(DatabaseResult& object,
                               ast::AssociationList* arguments,
                               Func sensitivityListCallback,
                               const bool double_brackets);
    
    ast::ObjectValueContainer getAttributeType(ast::ObjectValueContainer& type,
                                               std::string attributeName);
    
    bool findAttributeMatch(DatabaseResults& objects,
                            DatabaseResult& match,
                            ast::ObjectValueContainer& expectedType,
                            std::string& name);
    
    template <typename Func>
    std::string attributeToString(std::string name,
                                  ast::ObjectArguments& arguments,
                                  ast::ObjectValueContainer& expectedType,
                                  ast::Text* attribute,
                                  ast::AssociationList* associationList,
                                  Func sensitivityListCallback);
    template <typename Func>
    std::string associateArgument(ast::ObjectArgument& interfaceElement,
                                  int argumentNumber,
                                  ast::AssociationList* l,
                                  Func sensitivityListCallback); 
    template <typename Func>
    std::string parametersToString(ast::ObjectArguments& interface,
                                   ast::AssociationList* associationList,
                                   Func sensitivityListCallback);

    ast::ObjectArguments toObjectArguments(ast::AssociationList* associationList);


    std::string ReturnTypesToString(const ast::ReturnTypes& return_types);
    
    bool getStaticAttributeType(std::string attributeName, ast::ObjectValueContainer& result);

    bool exists(ReturnTypePair& pair, std::list<ReturnTypePair>& typePairs);
    
    bool objectWithArguments(DatabaseElement* e, ast::ObjectArguments& arguments,
                             ast::ObjectValueContainer* expectedReturnType = NULL);

  public:

    ExpressionParser(Database& database, NameConverter& name_converter, bool verbose = false) : debug("ExpressionParser") {
      a_database = &database;
      a_name_converter = &name_converter;
      this->verbose |= verbose;
    }
  
    std::string physicalToString(ast::Physical* physical);

    template <typename Func>
    std::string toString(ast::Expression* e, ast::ObjectValueContainer& expectedType, Func sensitivityListCallback);
    std::string toString(ast::Expression* e, ast::ObjectValueContainer& expectedType);
    template <typename Func>
    std::string toString(ast::Expression* e, ast::ObjectValue expectedType, Func sensitivityListCallback);
    std::string toString(ast::Expression* e, ast::ObjectValue expectedType);

    std::string procedureCallStatementToString(ast::ProcedureCallStatement* p);

    bool CollectAllReturnTypes(ast::Expression* e,
                               ast::ObjectValueContainer& expectedType);
    
    bool translateOperator(std::string& op, std::string& translatedOp);

    bool UniqueReturnType(ast::ReturnTypes& return_types, ast::ObjectValueContainer& type, ast::Text* text);
  
  };

  template <typename Func>
  std::string ExpressionParser::toString(ast::Expression* e,
                                         ast::ObjectValueContainer& expectedType,
                                         Func sensitivityListCallback) {
    debug.functionStart("toString");
    CollectAllReturnTypes(e, expectedType);
    bool double_brackets = !expectedType.IsArrayWithDimension(1);
    std::string s = expressionToString(e, expectedType, sensitivityListCallback, double_brackets);
    debug.functionEnd("toString");
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::toString(ast::Expression* expr,
                                         ast::ObjectValue expectedType,
                                         Func sensitivityListCallback) {
    debug.functionStart("toString");
    ast::ObjectValueContainer e(expectedType);
    std::string s = toString(expr, e, sensitivityListCallback);
    debug.functionEnd("toString");
    return s;
  }
    
  template<typename Func>
  void ExpressionParser::GetReturnTypes(const std::string& name, Func valid, ast::ReturnTypes& return_types) {
    debug.functionStart("GetReturnTypes(name = " + name + ")");
    DatabaseResults objects;
    a_database->findAll(objects, name);
    for (auto& i : objects) {
      return_types.insert(i.object->type);
    }
    debug.functionEnd("GetReturnTypes = " + ReturnTypesToString(return_types));
  }
  
  
  template <typename Func>
  std::string ExpressionParser::associateArgument(ast::ObjectArgument& interfaceElement,
                                                  int argumentNumber,
                                                  ast::AssociationList* l,
                                                  Func sensitivityListCallback) { 
    debug.functionStart("associateArgument, interfaceElement = " + interfaceElement.toString(), true);
    std::string argument = interfaceElement.default_value ? toString(interfaceElement.default_value, interfaceElement.type) : "";
    if (l) {
      int associationElementNumber = 0;
      for (auto& e : l->associationElements.list) {
        try {
          bool double_brackets = false;
          std::string actualPart = expressionToString(e.actualPart, interfaceElement.type, sensitivityListCallback, double_brackets);
          debug.debug("Actual part = " + actualPart);
          if (e.formalPart) {
            std::string formalPart = e.formalPart->toString(true);
            debug.debug("Formal part = " + formalPart + ", interface element = " + interfaceElement.name);
            if (formalPart == interfaceElement.name) {
              argument = actualPart;
              break;
            }
          } else if (associationElementNumber == argumentNumber) {
            debug.debug("Association number " + std::to_string(associationElementNumber) + " match");
            argument = actualPart;
            break;
          }
        } catch (ObjectNotFound e) {
        }
        associationElementNumber++;
      }
    }
    debug.functionEnd("associateArgument = " + argument);
    return argument;
  }

  template <typename Func>
  std::string ExpressionParser::parametersToString(ast::ObjectArguments& interface,
                                                   ast::AssociationList* associationList,
                                                   Func sensitivityListCallback) { 
    debug.functionStart("parametersToString");
    std::string s = "";
    /*
      Association list can either be:
      func(formalPart => actualPart, a => w, b => x, c => y)
      or
      func(actualPart, w, x, y)
      or array
      a(0)
    */
    std::string delimiter = "";
    int argumentNumber = 0;
    for (auto& i : interface.list) {
      std::string argument = associateArgument(i, argumentNumber, associationList, sensitivityListCallback);
      if (argument.size() == 0) {
        exceptions.printError("No argument associated element " + std::to_string(argumentNumber));
      }
      s += delimiter + argument;
      delimiter = ", ";
      argumentNumber++;
    };
    debug.functionEnd("parametersToString");
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::objectToString(DatabaseResult& object,
                                               ast::AssociationList* arguments,
                                               Func sensitivityListCallback,
                                               const bool double_brackets) {
    debug.functionStart("objectToString");
    assert(object.object);
    std::string name = a_name_converter->GetName(object);
    debug.debug("name = " + name + ": " + object.toString());
    if (object.object->type.IsValue(ast::ObjectValue::ARRAY)) {
      std::string parameters;
      if (arguments) {
        std::string delimiter = "";
        ast::ObjectValueContainer::Array& args = object.object->type.GetArguments();
        assert(args.size() == arguments->associationElements.list.size());
        auto it = args.begin();
        for (auto& i : arguments->associationElements.list) {
          parameters += delimiter + expressionToString(i.actualPart, *it, sensitivityListCallback, double_brackets);
          it++;
          delimiter = "][";
        }
        name += "[" + parameters + "]";
      }
    } else if (arguments) {
      std::string parameters = parametersToString(object.object->arguments, arguments, sensitivityListCallback);
      name += "(" + parameters + ")";
    } else if (object.object->id == ast::ObjectType::FUNCTION ||
               object.object->id == ast::ObjectType::PROCEDURE) {
      name += "()";
    }
    debug.functionEnd("objectToString = " + name);
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::operationToString(ast::Expression* e,
                                                  ast::ObjectValueContainer& expectedType,
                                                  Func sensitivityListCallback) {
    debug.functionStart("operationToString");
    std::string result;
    std::list<ReturnTypePair> typePairs;
    ast::ReturnTypes t;
    for (auto& i : e->term->returnTypes) {
      for (auto& j : e->expression->returnTypes) {
        t.clear();
        OperatorReturnTypes(e->op->op, i, j, t);
        for (auto& x : t) {
          if (x.equals(expectedType)) {
            ReturnTypePair pair = {i, j};
            if (!exists(pair, typePairs)) {
              typePairs.push_back(pair);
            }
          }
        }
      }
    }
    if (typePairs.size() != 1) {
      exceptions.printError("Cound not resolve expected type " + expectedType.toString(), e->text);
      if (typePairs.empty()) {
        std::cerr << "Did not find anything" << std::endl;
      } else {
        std::cerr << "Found the following types:" << std::endl;
        for (auto& i : typePairs) {
          std::cerr << i.left.toString() + " " + e->op->op + " " + i.right.toString() + " = " + expectedType.toString() << std::endl;
        }
      }
    } else {
      bool double_brackets = false;
      std::string term = expressionTermToString(e->term, typePairs.back().left, sensitivityListCallback, double_brackets);
      std::string expr = expressionToString(e->expression, typePairs.back().right, sensitivityListCallback, double_brackets);
      std::string op;
      translateOperator(e->op->op, op);
      result = term + " " + op + " " + expr;
    }
    debug.functionEnd("operationToString");
    return result;
  }
  
  template <typename Func>
  std::string ExpressionParser::expressionToString(ast::Expression* e,
                                                   ast::ObjectValueContainer& expectedType,
                                                   Func sensitivityListCallback,
                                                   const bool double_brackets) {
    debug.functionStart("expressionToString");
    assert(e);
    std::string result;
    if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(e->expression, expectedType, sensitivityListCallback, double_brackets);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      case ::ast::UnaryOperator::MINUS: {op = "-"; break;}
      default: {assert (false);}
      }
      result = op + expr;
    } else if (e->op) {
      result = operationToString(e, expectedType, sensitivityListCallback);
    } else {
      result = expressionTermToString(e->term, expectedType, sensitivityListCallback, double_brackets);
    }
    debug.functionEnd("expressionToString = " + result);
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::expressionTermToString(ast::ExpressionTerm* e,
                                                       ast::ObjectValueContainer& expectedType,
                                                       Func sensitivityListCallback,
                                                       const bool double_brackets) {
    std::string result = "";
    if (e) {
      debug.functionStart("expressionTermToString");
      if (!e->parenthis.list.empty()) {
        bool array_type = expectedType.IsValue(ast::ObjectValue::ARRAY);
        std::string d;
        for (ast::Expression& i : e->parenthis.list) {
          result += d + expressionToString(&i, expectedType, sensitivityListCallback, double_brackets);
          d = ", ";
        }
        std::string left = array_type ? (double_brackets ? "{{" : "{") : "(";
        std::string right = array_type ? (double_brackets ? "}}" : "}") : ")";
        result = left + result + right;
      } else if (e->physical) {
        debug.debug("Physical");
        result = physicalToString(e->physical);
      } else if (e->number) {
        debug.debug("Number");
        result = e->number->toString();
      } else if (e->string) {
        debug.debug("String");
        result = e->string->toString();
      } else if (e->identifier) {
        result = BasicIdentifierToString(e->identifier, &expectedType, sensitivityListCallback);
      } else if (e->character) {
        debug.debug("Character");
        result = e->character->toString();
      } else {
        assert(false);
      }
      debug.functionEnd("expressionTermToString");
    }
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::BasicIdentifierToString(ast::BasicIdentifier* identifier,
                                                        ast::ObjectValueContainer* expected_type,
                                                        Func sensitivityListCallback) {
    std::string name = identifier->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(identifier->arguments);
    debug.functionStart("BasicIdentifierToString(name = " + name + (arguments.empty() ? "" : "(" + arguments.toString() + ")") +
                        ", expected_type = " + (expected_type ? expected_type->toString() : "None") + ")", true);
    if (identifier->attribute) {
      name = attributeToString(name, arguments, *expected_type, identifier->attribute,
                               identifier->arguments, sensitivityListCallback);
    } else {
      auto valid = [&](DatabaseElement* e) {
        bool result = objectWithArguments(e, arguments, expected_type);
        return result;
      };
      DatabaseResult object;
      if (a_database->findOne(object, name, valid)) {
        if (object.object->id == ast::ObjectType::SIGNAL) {
          sensitivityListCallback(object);
        }
        bool double_brackets = false;
        name = objectToString(object, identifier->arguments, sensitivityListCallback, double_brackets);
      } else {
	std::string args = arguments.toString();
	args = args.empty() ? "" : "(" + args + ")";
        throw ObjectNotFound(this, name, "Could not find definition of " + name + args +
                             " with type " + (expected_type ? expected_type->toString() : "None"), &identifier->text);
      }
    } 
    debug.functionEnd("BasicIdentifierToString = " + name);
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::attributeToString(std::string name,
                                                  ast::ObjectArguments& arguments,
                                                  ast::ObjectValueContainer& expectedType,
                                                  ast::Text* attribute,
                                                  ast::AssociationList* associationList,
                                                  Func sensitivityListCallback) {
    assert(attribute);
    auto valid = [&](DatabaseElement* e) {
      return e->arguments.equals(arguments);
    };
    DatabaseResults objects;
    a_database->findAll(objects, name);
    std::string attributeName = attribute->toString(true);
    DatabaseResult match;
    if (findAttributeMatch(objects, match, expectedType, attributeName)) {
      assert(match.object);
      bool objectMatch = (match.object->id == ast::ObjectType::VARIABLE) ||
        (match.object->id == ast::ObjectType::SIGNAL);
      name = a_name_converter->GetName(match, true) + "." + attributeName;
      std::string a = "";
      if (associationList) {
        std::string delimiter = "";
        bool double_brackets = false;
        for (auto& i : associationList->associationElements.list) {
          a = delimiter + expressionToString(i.actualPart, match.object->type, sensitivityListCallback, double_brackets);
          delimiter = ", ";
        }
      }
      name += "(" + a + ")";
    } else {
      exceptions.printError("Could not find match for attribute \"" + attributeName +
                            "\" with expected type " + expectedType.toString(), attribute);
    }
    return name;
  }

}


#endif

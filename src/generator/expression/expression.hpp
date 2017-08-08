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
    
    using ReturnTypes = std::list<ast::ObjectValueContainer>;

    // Exception classes

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
    
    template<typename Func>
    ReturnTypes getReturnTypes(std::string& name, Func valid); 
    ReturnTypes getStandardOperatorReturnTypes(std::string& name,
					       ast::ObjectValueContainer& l,
					       ast::ObjectValueContainer& r);
   
    // First-pass functions
    ReturnTypes expressionReturnTypes(ast::Expression* e);   
    ReturnTypes expressionTermReturnTypes(ast::ExpressionTerm* e);  
    ReturnTypes basicIdentifierReturnTypes(ast::BasicIdentifier* b);
    ReturnTypes operatorReturnTypes(std::string& name,
                                    ast::ObjectValueContainer& l,
                                    ast::ObjectValueContainer& r);
    ReturnTypes functionReturnTypes(std::string& name, ast::AssociationList* associationList);
    ReturnTypes attributeReturnTypes(std::string& name, std::string& attribute, ast::AssociationList* associationList);
    
    // Second-pass functions 
    template <typename Func>
    std::string operationToString(ast::Expression* e,
                                  ast::ObjectValueContainer& expectedType,
                                  Func sensitivityListCallback);
    template <typename Func>
    std::string expressionToString(ast::Expression* e,
                                   ast::ObjectValueContainer& expectedType,
                                   Func sensitivityListCallback);
    template <typename Func>
    std::string expressionTermToString(ast::ExpressionTerm* e,
                                       ast::ObjectValueContainer& expectedType,
                                       Func sensitivityListCallback);
    template <typename Func>
    std::string basicIdentifierToString(ast::BasicIdentifier* identifier,
                                        ast::ObjectValueContainer& expectedType,
                                        Func sensitivityListCallback);

    template <typename Func>
    std::string objectToString(DatabaseResult& object,
                               ast::AssociationList* arguments,
                               Func sensitivityListCallback);
    
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


    std::string returnTypesToString(ReturnTypes& returnTypes);
    
    bool getStaticAttributeType(std::string attributeName, ast::ObjectValueContainer& result);

    bool exists(ReturnTypePair& pair, std::list<ReturnTypePair>& typePairs);
    
    bool objectWithArguments(DatabaseElement* e, ast::ObjectArguments& arguments,
                             ast::ObjectValueContainer* expectedReturnType = NULL);

    void removeDuplicates(ReturnTypes& r);
    
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

    bool getType(ast::Expression* e,
                 ast::ObjectValueContainer& expectedType,
                 ast::ObjectValueContainer& actualType);
    
    bool translateOperator(std::string& op, std::string& translatedOp);

  };

  template <typename Func>
  std::string ExpressionParser::toString(ast::Expression* e,
                                         ast::ObjectValueContainer& expectedType,
                                         Func sensitivityListCallback) {
    debug.functionStart("toString");
    // First-pass: Collect the possible return types
    ast::ObjectValueContainer actualType;
    getType(e, expectedType, actualType); 
    // debug.debug("Expected type = " + expectedType.toString() + ", actual type = " + actualType.toString());
    // Second-pass: Resolve the return type and convert to string
    std::string s = expressionToString(e, actualType, sensitivityListCallback);
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
  ExpressionParser::ReturnTypes ExpressionParser::getReturnTypes(std::string& name, Func valid) {
    debug.functionStart("getReturnTypes");
    DatabaseResults objects;
    a_database->findAll(objects, name, valid);
    ReturnTypes result;
    for (auto& i : objects) {
      result.push_back(i.object->type);
    }
    debug.functionEnd("getReturnTypes");
    return result;
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
          std::string actualPart = expressionToString(e.actualPart, interfaceElement.type, sensitivityListCallback);
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
                                               Func sensitivityListCallback) {
    debug.functionStart("objectToString");
    assert(object.object);
    std::string name = a_name_converter->getName(object, true);
    debug.debug("name = " + name + ": " + object.toString());
    if (object.object->type.value == ast::ObjectValue::ARRAY) {
      std::string parameters;
      if (arguments) {
        std::string delimiter = "";
        ast::ObjectValueContainer* subtype = object.object->type.subtype;
        assert(subtype);
        for (auto& i : arguments->associationElements.list) {
          parameters = delimiter + expressionToString(i.actualPart, *subtype, sensitivityListCallback);
          delimiter = ", ";
        }
      }
      name += "[" + parameters + "]";
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
    for (auto& i : e->term->returnTypes) {
      for (auto& j : e->expression->returnTypes) {
        ReturnTypes t = operatorReturnTypes(e->op->op, i, j);
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
      std::string term = expressionTermToString(e->term, typePairs.back().left, sensitivityListCallback);
      std::string expr = expressionToString(e->expression, typePairs.back().right, sensitivityListCallback);
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
                                                   Func sensitivityListCallback) {
    debug.functionStart("expressionToString");
    assert(e);
    std::string result;
    if (e->parenthis) {
      result = "(" + expressionToString(e->parenthis, expectedType, sensitivityListCallback) + ")";
    } else if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(e->expression, expectedType, sensitivityListCallback);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      case ::ast::UnaryOperator::MINUS: {op = "-"; break;}
      default: {assert (false);}
      }
      result = op + expr;
    } else if (e->op) {
      result = operationToString(e, expectedType, sensitivityListCallback);
    } else {
      result = expressionTermToString(e->term, expectedType, sensitivityListCallback);
    }
    debug.functionEnd("expressionToString = " + result);
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::expressionTermToString(ast::ExpressionTerm* e,
                                                       ast::ObjectValueContainer& expectedType,
                                                       Func sensitivityListCallback) {
    std::string result = "";
    if (e) {
      debug.functionStart("expressionTermToString");
      if (e->physical) {
        debug.debug("Physical");
        result = physicalToString(e->physical);
      } else if (e->number) {
        debug.debug("Number");
        result = e->number->toString();
      } else if (e->string) {
        debug.debug("String");
        result = e->string->toString();
      } else if (e->identifier) {
        result = basicIdentifierToString(e->identifier, expectedType, sensitivityListCallback);
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
  std::string ExpressionParser::basicIdentifierToString(ast::BasicIdentifier* identifier,
                                                        ast::ObjectValueContainer& expectedType,
                                                        Func sensitivityListCallback) {
    debug.functionStart("basicIdentifierToString");
    std::string name = identifier->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(identifier->arguments);
    if (identifier->attribute) {
      name = attributeToString(name, arguments, expectedType, identifier->attribute,
                               identifier->arguments, sensitivityListCallback);
    } else {
      auto valid = [&](DatabaseElement* e) {
        bool result = objectWithArguments(e, arguments, &expectedType);
        return result;
      };
      DatabaseResult object;
      if (a_database->findOne(object, name, valid)) {
        if (object.object->id == ast::ObjectType::SIGNAL) {
          sensitivityListCallback(object);
        }
        name = objectToString(object, identifier->arguments, sensitivityListCallback);
      } else {
	std::string args = arguments.toString();
	args = args.empty() ? "" : "(" + args + ")";
        throw ObjectNotFound(this, name, "Could not find definition of " + name + args +
                             " with type " + expectedType.toString(), &identifier->text);
      }
    } 
    debug.functionEnd("basicIdentifierToString = " + name);
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
    a_database->findAll(objects, name, valid);
    std::string attributeName = attribute->toString(true);
    DatabaseResult match;
    if (findAttributeMatch(objects, match, expectedType, attributeName)) {
      assert(match.object);
      bool objectMatch = (match.object->id == ast::ObjectType::VARIABLE) ||
        (match.object->id == ast::ObjectType::SIGNAL);
      std::string seperator = objectMatch ? "." : "::";
      name = a_database->globalName(name) + seperator + attributeName;
      std::string a = "";
      if (associationList) {
        std::string delimiter = "";
        for (auto& i : associationList->associationElements.list) {
          a = delimiter + expressionToString(i.actualPart, match.object->type, sensitivityListCallback);
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

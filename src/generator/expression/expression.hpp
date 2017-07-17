#ifndef _GENERATOR_EXPRESSION_PARSER_HPP
#define _GENERATOR_EXPRESSION_PARSER_HPP

#include <list>
#include <cassert>

#include "../../ast/object_type.hpp"
#include "../../exceptions/exceptions.hpp"
#include "../database/database.hpp"

namespace generator {

  class ExpressionParser {

    bool verbose = false;
    Database* database;

    void functionStart(std::string name);
    void functionEnd(std::string name);

    Exceptions exceptions;

    struct ReturnTypePair {
      ast::ObjectValueContainer left;
      ast::ObjectValueContainer right;
    };
    
    using ReturnTypes = std::list<ast::ObjectValueContainer>;

    template<typename Func>
    ReturnTypes getReturnTypes(std::string& name, Func valid);
    
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

    std::string physicalToString(ast::Physical* physical);

    std::string returnTypesToString(ReturnTypes& returnTypes);
    
    bool getStaticAttributeType(std::string attributeName, ast::ObjectValueContainer& result);
    
  public:

    ExpressionParser(Database* database, bool verbose = false) : database(database) {
      this->verbose |= verbose;
    }
  
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
    functionStart("toString");
    // First-pass: Collect the possible return types
    ast::ObjectValueContainer actualType;
    getType(e, expectedType, actualType);
    // Second-pass: Resolve the return type and convert to string
    std::string s = expressionToString(e, actualType, sensitivityListCallback);
    functionEnd("toString");
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::toString(ast::Expression* expr,
                                         ast::ObjectValue expectedType,
                                         Func sensitivityListCallback) {
    functionStart("toString");
    ast::ObjectValueContainer e(expectedType);
    std::string s = toString(expr, e, sensitivityListCallback);
    functionEnd("toString");
    return s;
  }
    
  template<typename Func>
  ExpressionParser::ReturnTypes ExpressionParser::getReturnTypes(std::string& name, Func valid) {
    DatabaseResults objects;
    database->findAll(objects, name, valid);
    ReturnTypes result;
    for (auto& i : objects) {
      result.push_back(i.object->type);
    }
    return result;
  }
  
  
  template <typename Func>
  std::string ExpressionParser::associateArgument(ast::ObjectArgument& interfaceElement,
                                                  int argumentNumber,
                                                  ast::AssociationList* l,
                                                  Func sensitivityListCallback) { 
    functionStart("associateArgument");
    std::string argument = interfaceElement.defaultValue;
    if (l) {
      int associationElementNumber = 0;
      for (auto& e : l->associationElements.list) {
        std::string actualPart = expressionToString(e.actualPart, interfaceElement.type, sensitivityListCallback);
        if (e.formalPart) {
          std::string formalPart = e.formalPart->toString(true);
          if (formalPart == interfaceElement.name) {
            return actualPart;
          }
        } else if (associationElementNumber == argumentNumber) {
          return actualPart;
        }
        associationElementNumber++;
      }
    }
    functionEnd("associateArgument");
    return argument;
  }

  template <typename Func>
  std::string ExpressionParser::parametersToString(ast::ObjectArguments& interface,
                                                   ast::AssociationList* associationList,
                                                   Func sensitivityListCallback) { 
    functionStart("parametersToString");
    std::string s = "";
    /*
      Association list can either be:
      func(formalPart => actualPart, a => w, b => x, c => y)
      or
      func(actualPart, w, x, y)
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
    functionEnd("parametersToString");
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::objectToString(DatabaseResult& object,
                                               ast::AssociationList* arguments,
                                               Func sensitivityListCallback) {
    functionStart("objectToString");
    assert(object.object);
    std::string name = object.getName(true, database->getLibrary(), database->getSection());
    if (verbose) {
      std::cout << "name = " << name << ": " << object.toString() << std::endl;
    }
    if (arguments) {
      assert(object.object->id == ast::FUNCTION || object.object->id == ast::PROCEDURE);
      std::string parameters = parametersToString(object.object->arguments, arguments, sensitivityListCallback);
      name += "(" + parameters + ")";
    }
    functionEnd("objectToString");
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::expressionToString(ast::Expression* e,
                                                   ast::ObjectValueContainer& expectedType,
                                                   Func sensitivityListCallback) {
    functionStart("expressionToString");
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
      std::list<ReturnTypePair> typePairs;
      for (auto& i : e->term->returnTypes) {
        for (auto& j : e->expression->returnTypes) {
          ReturnTypes t = operatorReturnTypes(e->op->op, i, j);
          for (auto& x : t) {
            if (x.equals(expectedType)) {
              typePairs.push_back({i, j});
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
    } else {
      result = expressionTermToString(e->term, expectedType, sensitivityListCallback);
    }
    functionEnd("expressionToString");
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::expressionTermToString(ast::ExpressionTerm* e,
                                                       ast::ObjectValueContainer& expectedType,
                                                       Func sensitivityListCallback) {
    if (e) {
      if (e->physical) {
        return physicalToString(e->physical);
      }
      if (e->number) {
        return e->number->toString();
      }
      if (e->string) {
        return e->string->toString();
      }
      if (e->identifier) {
        return basicIdentifierToString(e->identifier, expectedType, sensitivityListCallback);
      }
      if (e->character) {
        return e->character->toString();
      }
    }
    return "";
  }

  template <typename Func>
  std::string ExpressionParser::basicIdentifierToString(ast::BasicIdentifier* identifier,
                                                        ast::ObjectValueContainer& expectedType,
                                                        Func sensitivityListCallback) {
    // functionStart("basicIdentifierToString");
    std::string name = identifier->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(identifier->arguments);
    if (identifier->attribute) {
      return attributeToString(name, arguments, expectedType, identifier->attribute,
                               identifier->arguments, sensitivityListCallback);
    } else {
      auto valid = [&](DatabaseElement* e) {
        return (e->arguments.equals(arguments) && e->type.equals(expectedType)) ? true : false;
      };
      DatabaseResult object;
      if (database->findOne(object, name, valid)) {
        if (object.object->id == ast::SIGNAL) {
          sensitivityListCallback(object);
        }
        return objectToString(object, identifier->arguments, sensitivityListCallback);
      } else {
	std::string args = arguments.toString();
	args = args.empty() ? "" : "(" + args + ")";
        exceptions.printError("Could not find definition of \"" + name + args +
			      "\" with type " + expectedType.toString(), &identifier->text);
	database->printAllObjects(name);
      }
    } 
    // functionEnd("basicIdentifierToString");
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
    database->findAll(objects, name, valid);
    std::string attributeName = attribute->toString(true);
    DatabaseResult match;
    if (findAttributeMatch(objects, match, expectedType, attributeName)) {
      assert(match.object);
      bool objectMatch = (match.object->id == ast::VARIABLE) || (match.object->id == ast::SIGNAL);
      std::string seperator = objectMatch ? "." : "<>::";
      name = name + seperator + attributeName;
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

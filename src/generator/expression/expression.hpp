#ifndef _GENERATOR_EXPRESSION_PARSER_HPP
#define _GENERATOR_EXPRESSION_PARSER_HPP

#include <list>
#include <cassert>

#include "../../ast/object_type.hpp"
#include "../../exceptions/exceptions.hpp"
#include "../database/database.hpp"

namespace generator {

  class ExpressionParser {

    Database* database;

    Exceptions exceptions;
    
    using ReturnTypes = std::list<ast::ObjectValueContainer>;
    
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
    std::string basicIdentifierToString(ast::BasicIdentifier* b,
                                        ast::ObjectValueContainer& expectedType,
                                        Func sensitivityListCallback);

    std::string associateArgument(std::string& name, std::string& init,
                                  int argumentNumber, ast::AssociationList* l); 

    std::string parametersToString(ast::BasicIdentifier* name,
                                   ast::InterfaceList* interface,
                                   ast::AssociationList* associationList);

    bool match(DatabaseResult& databaseResult, ast::ObjectArguments& arguments);
    ReturnTypes match(DatabaseResults& databaseResults, ast::ObjectArguments& arguments);
      
    ast::ObjectArguments toObjectArguments(ast::AssociationList* associationList);

  public:

    ExpressionParser(Database* database) : database(database) {}
  
    template <typename Func>
    std::string toString(ast::Expression* e, ast::ObjectValueContainer& expectedType, Func sensitivityListCallback);
    std::string toString(ast::Expression* e, ast::ObjectValueContainer& expectedType);
    
  };

  template <typename Func>
  std::string ExpressionParser::toString(ast::Expression* e,
                                         ast::ObjectValueContainer& expectedType,
                                         Func sensitivityListCallback) {
    // First-pass: Collect the possible return types
    ReturnTypes o = expressionReturnTypes(e);
    // Second-pass: Resolve the return type and convert to string
    return expressionToString(e, expectedType, sensitivityListCallback);
  }

  std::string ExpressionParser::toString(ast::Expression* e, ast::ObjectValueContainer& expectedType) {
    return toString(e, expectedType, [&](std::string& name) {});
  }

  bool ExpressionParser::match(DatabaseResult& databaseResult, ast::ObjectArguments& arguments) {
    ast::ObjectArguments& a = databaseResult.object->arguments;
    return ast::match(a, arguments);
  }
  
  ExpressionParser::ReturnTypes ExpressionParser::match(DatabaseResults& databaseResults, ast::ObjectArguments& arguments) {
    ReturnTypes result;
    for (auto i : databaseResults) {
      if (match(i, arguments)) {
        result.push_back(i.object->type);
      }
    }
    return result;
  };
  
  ExpressionParser::ReturnTypes ExpressionParser::operatorReturnTypes(std::string& name,
                                                                      ast::ObjectValueContainer& l,
                                                                      ast::ObjectValueContainer& r) {
    DatabaseResults databaseResults;
    database->find(databaseResults, name);
    ast::ObjectArguments x = {ast::ObjectArgument(l), ast::ObjectArgument(r)};
    return match(databaseResults, x);
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
    ast::ObjectArguments a;
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
      DatabaseResults databaseResults;
      database->find(databaseResults, name);
      ast::ObjectArguments a;
      return match(databaseResults, a);
    }
  }
  
  template <typename Func>
  std::string ExpressionParser::expressionToString(ast::Expression* e,
                                                   ast::ObjectValueContainer& expectedType,
                                                   Func sensitivityListCallback) {
    assert(e);
    if (e->parenthis) {
      return "(" + expressionToString(e->parenthis, expectedType, sensitivityListCallback) + ")";
    } else if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(e->expression, sensitivityListCallback);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      case ::ast::UnaryOperator::MINUS: {op = "-"; break;}
      default: {assert (false);}
      }
      return op + expr;
    } else if (e->op) {
      std::string op;
      std::string term = expressionTermToString(e->term, sensitivityListCallback);
      std::string expr = expressionToString(e->expression, sensitivityListCallback);
      return term + " " + op + " " + expr;
    } else {
      return expressionTermToString(e->term, sensitivityListCallback);
    }
  }

  template <typename Func>
  std::string ExpressionParser::expressionTermToString(ast::ExpressionTerm* e,
                                                       ast::ObjectValueContainer& expectedType,
                                                       Func sensitivityListCallback) {
    if (e) {
      if (e->physical) {
        return e->physical->toString();
      }
      if (e->number) {
        return e->number->toString();
      }
      if (e->text) {
        return e->text->toString();
      }
      if (e->identifier) {
        return basicIdentifierToString(expectedType, e->identifier, sensitivityListCallback);
      }
      if (e->character) {
        return e->character->toString();
      }
    }
    return "";
  }

}


#endif

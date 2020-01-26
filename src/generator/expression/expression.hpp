#ifndef _GENERATOR_EXPRESSION_PARSER_HPP
#define _GENERATOR_EXPRESSION_PARSER_HPP

#include <list>
#include <cassert>

#include "../../ast/object_type.hpp"
#include "../../exceptions/exceptions.hpp"
#include "../../debug/debug.hpp"
#include "../parameters.hpp"
#include "../name_converter.hpp"

namespace generator {

  class ExpressionParser {

    Debug<false> debug;
    
    bool verbose = false;
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
      }
    };

  private:
    template<typename Func>
    void getReturnTypes(parameters& parm,
			std::string& name,
			Func valid,
			ast::ReturnTypes& return_types); 
    void getStandardOperatorReturnTypes(parameters& parm,
					std::string& name,
					const ast::ObjectValueContainer& l,
					const ast::ObjectValueContainer& r,
					ast::ReturnTypes& return_types);
   
    // First-pass functions
    void expressionReturnTypes(parameters& parm,
			       ast::Expression* e,
			       ast::ReturnTypes& return_types);   
    void parenthesisReturnTypes(parameters& parm,
				ast::ExpressionTerm* e);  
    void expressionTermReturnTypes(parameters& parm,
				   ast::ExpressionTerm* e,
				   ast::ReturnTypes& return_types);  
  public:
    void basicIdentifierReturnTypes(parameters& parm, ast::BasicIdentifier* b, ast::ReturnTypes& return_types);
  private:
    void operatorReturnTypes(parameters& parm,
			     std::string& name,
			     const ast::ObjectValueContainer& l,
			     const ast::ObjectValueContainer& r,
			     ast::ReturnTypes& return_types);
    void functionReturnTypes(parameters& parm,
			     std::string& name,
			     ast::AssociationList* associationList,
			     ast::ReturnTypes& return_types);
    void attributeReturnTypes(parameters& parm,
			      std::string& name,
			      std::string& attribute,
			      ast::AssociationList* associationList, ast::ReturnTypes& return_types);
    
    // Second-pass functions 
    template <typename Func>
    std::string operationToString(parameters& parm,
				  ast::Expression* e,
				  ast::ObjectValueContainer& expectedType,
				  Func sensitivityListCallback);
    template <typename Func>
    std::string expressionToString(parameters& parm,
				   ast::Expression* e,
				   ast::ObjectValueContainer& expectedType,
				   Func sensitivityListCallback,
				   bool double_brackets);
    template <typename Func>
    std::string expressionTermToString(parameters& parm,
				       ast::ExpressionTerm* e,
				       ast::ObjectValueContainer& expectedType,
				       Func sensitivityListCallback,
				       bool double_brackets);
  public:
    template <typename Func>
    std::string basicIdentifierToString(parameters& parm,
					ast::BasicIdentifier* identifier,
					ast::ObjectValueContainer* expectedType,
					Func sensitivityListCallback);
    std::string basicIdentifierToString(parameters& parm,
					ast::BasicIdentifier* identifier,
					ast::ObjectValueContainer* expectedType = NULL);
  private:
    template <typename Func>
    std::string objectToString(parameters& parm,
			       DatabaseResult& object,
			       ast::AssociationList* arguments,
			       Func sensitivityListCallback,
			       bool double_brackets);
    
    ast::ObjectValueContainer getAttributeType(parameters& parm,
					       ast::ObjectValueContainer& type,
					       std::string attributeName);
    
    bool findAttributeMatch(parameters& parm,
			    DatabaseResults& objects,
			    DatabaseResult& match,
			    ast::ObjectValueContainer& expectedType,
			    std::string& name);
    
    template <typename Func>
    std::string attributeToString(parameters& parm,
				  std::string name,
				  ast::ObjectArguments& arguments,
				  ast::ObjectValueContainer& expectedType,
				  ast::Text* attribute,
				  ast::AssociationList* associationList,
				  Func sensitivityListCallback);
    template <typename Func>
    std::string associateArgument(parameters& parm,
				  ast::ObjectArgument& interfaceElement,
				  int argumentNumber,
				  ast::AssociationList* l,
				  Func sensitivityListCallback); 
    template <typename Func>
    std::string parametersToString(parameters& parm,
				   ast::ObjectArguments& interface,
				   ast::AssociationList* associationList,
				   Func sensitivityListCallback);

    ast::ObjectArguments toObjectArguments(parameters& parm,
					   ast::AssociationList* associationList);


    std::string returnTypesToString(parameters& parm,
				    ast::ReturnTypes& return_types);
    
    bool getStaticAttributeType(parameters& parm,
				std::string attributeName,
				ast::ObjectValueContainer& result);
    
    bool exists(parameters& parm,
		ReturnTypePair& pair,
		std::list<ReturnTypePair>& typePairs);
    
    bool objectWithArguments(parameters& parm,
			     DatabaseElement* e, ast::ObjectArguments& arguments,
			     ast::ObjectValueContainer* expectedReturnType = NULL);

  public:

    ExpressionParser(bool verbose = false) : debug("ExpressionParser") {
      this->verbose |= verbose;
    }
  
    std::string physicalToString(parameters& parm,
				 ast::Physical* physical);

    template <typename Func>
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ast::ObjectValueContainer& expectedType,
			 Func sensitivityListCallback);
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ast::ObjectValueContainer& expectedType);
    template <typename Func>
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ast::ObjectValue expectedType,
			 Func sensitivityListCallback);
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ast::ObjectValue expectedType);

    std::string procedureCallStatementToString(parameters& parm,
					       ast::ProcedureCallStatement* p);

    bool collectAllReturnTypes(parameters& parm,
			       ast::Expression* e,
			       ast::ObjectValueContainer& expectedType);

    bool collectUniqueReturnType(parameters& parm,
				 ast::Expression* e,
				 ast::ObjectValueContainer& type);
    
    std::string translateOperator(parameters& parm,
				  std::string& op);

    bool uniqueReturnType(parameters& parm,
			  ast::ReturnTypes& return_types,
			  ast::ObjectValueContainer& type,
			  ast::Text* text);
  
  };

  template <typename Func>
  std::string ExpressionParser::toString(parameters& parm,
					 ast::Expression* e,
                                         ast::ObjectValueContainer& expectedType,
                                         Func sensitivityListCallback) {
    debug.functionStart("toString");
    collectAllReturnTypes(parm, e, expectedType);
    bool double_brackets = !expectedType.IsArrayWithDimension(1);
    std::string s = expressionToString(parm, e, expectedType, sensitivityListCallback, double_brackets);
    debug.functionEnd("toString");
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::toString(parameters& parm,
					 ast::Expression* expr,
                                         ast::ObjectValue expectedType,
                                         Func sensitivityListCallback) {
    debug.functionStart("toString");
    ast::ObjectValueContainer e(expectedType);
    std::string s = toString(parm, expr, e, sensitivityListCallback);
    debug.functionEnd("toString");
    return s;
  }
    
  template<typename Func>
  void ExpressionParser::getReturnTypes(parameters& parm, std::string& name, Func valid, ast::ReturnTypes& return_types) {
    debug.functionStart("GetReturnTypes(name = " + name + ")");
    DatabaseResults objects;
    parm.findAll(objects, name);
    for (auto& i : objects) {
      return_types.insert(i.object->type);
    }
    debug.functionEnd("GetReturnTypes = " + returnTypesToString(parm, return_types));
  }
  
  
  template <typename Func>
  std::string ExpressionParser::associateArgument(parameters& parm,
						  ast::ObjectArgument& interfaceElement,
                                                  int argumentNumber,
                                                  ast::AssociationList* l,
                                                  Func sensitivityListCallback) { 
    debug.functionStart("associateArgument, interfaceElement = " + interfaceElement.toString(), true);
    std::string argument = interfaceElement.default_value ? toString(parm, interfaceElement.default_value, interfaceElement.type) : "";
    if (l) {
      int associationElementNumber = 0;
      for (auto& e : l->associationElements.list) {
        try {
          bool double_brackets = false;
          std::string actualPart = expressionToString(parm, e.actualPart, interfaceElement.type, sensitivityListCallback, double_brackets);
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
  std::string ExpressionParser::parametersToString(parameters& parm,
						   ast::ObjectArguments& interface,
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
      std::string argument = associateArgument(parm, i, argumentNumber, associationList, sensitivityListCallback);
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
  std::string ExpressionParser::objectToString(parameters& parm,
					       DatabaseResult& object,
                                               ast::AssociationList* arguments,
                                               Func sensitivityListCallback,
                                               bool double_brackets) {
    debug.functionStart("objectToString");
    assert(object.object);
    std::string name = NameConverter::getName(parm, object);
    debug.debug("name = " + name + ": " + object.toString());
    if (object.object->type.IsValue(ast::ObjectValue::ARRAY)) {
      if (arguments) {
        bool cast_operator = (object.object->id == ast::ObjectType::TYPE);
        if (cast_operator) {
          assert(arguments->associationElements.list.size() == 1);
          ast::Expression* arg = arguments->associationElements.list.front().actualPart;
          name += "(" + expressionToString(parm, arg, object.object->type, sensitivityListCallback, double_brackets) + ")";
        } else {
          std::string parameters;
          std::string delimiter = "";
          ast::ObjectValueContainer::Array& args = object.object->type.GetArguments();
          assert(args.size() == arguments->associationElements.list.size());
          auto it = args.begin();
          for (auto& i : arguments->associationElements.list) {
            parameters += delimiter + expressionToString(parm, i.actualPart, *it, sensitivityListCallback, double_brackets);
            it++;
            delimiter = "][";
          }
          name += "[" + parameters + "]";
        }
      }
    } else if (arguments) {
      std::string parameters = parametersToString(parm, object.object->arguments, arguments, sensitivityListCallback);
      name += "(" + parameters + ")";
    } else if (object.object->id == ast::ObjectType::FUNCTION ||
               object.object->id == ast::ObjectType::PROCEDURE) {
      name += "()";
    }
    debug.functionEnd("objectToString = " + name);
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::operationToString(parameters& parm,
						  ast::Expression* e,
                                                  ast::ObjectValueContainer& expectedType,
                                                  Func sensitivityListCallback) {
    debug.functionStart("operationToString");
    std::string result;
    std::list<ReturnTypePair> typePairs;
    ast::ReturnTypes t;
    for (auto& i : e->term->returnTypes) {
      for (auto& j : e->expression->returnTypes) {
        t.clear();
        operatorReturnTypes(parm, e->op->op, i, j, t);
        for (auto& x : t) {
          if (x.equals(expectedType)) {
            ReturnTypePair pair = {i, j};
            if (!exists(parm, pair, typePairs)) {
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
      std::string term = expressionTermToString(parm, e->term, typePairs.back().left, sensitivityListCallback, double_brackets);
      std::string expr = expressionToString(parm, e->expression, typePairs.back().right, sensitivityListCallback, double_brackets);
      std::string op;
      op = translateOperator(parm, e->op->op);
      result = term + " " + op + " " + expr;
    }
    debug.functionEnd("operationToString");
    return result;
  }
  
  template <typename Func>
  std::string ExpressionParser::expressionToString(parameters& parm,
						   ast::Expression* e,
                                                   ast::ObjectValueContainer& expectedType,
                                                   Func sensitivityListCallback,
                                                   bool double_brackets) {
    debug.functionStart("expressionToString");
    assert(e);
    std::string result;
    if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(parm, e->expression, expectedType, sensitivityListCallback, double_brackets);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      case ::ast::UnaryOperator::MINUS: {op = "-"; break;}
      default: {assert (false);}
      }
      result = op + expr;
    } else if (e->op) {
      result = operationToString(parm, e, expectedType, sensitivityListCallback);
    } else {
      result = expressionTermToString(parm, e->term, expectedType, sensitivityListCallback, double_brackets);
    }
    debug.functionEnd("expressionToString = " + result);
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::expressionTermToString(parameters& parm,
						       ast::ExpressionTerm* e,
                                                       ast::ObjectValueContainer& expectedType,
                                                       Func sensitivityListCallback,
                                                       bool double_brackets) {
    std::string result = "";
    if (e) {
      debug.functionStart("expressionTermToString");
      if (!e->parenthis.list.empty()) {
        bool array_type = expectedType.IsValue(ast::ObjectValue::ARRAY);
        std::string d;
        for (ast::ElementAssociation& i : e->parenthis.list) {
          ast::Expression* expression = i.expression;
          if (expression == NULL) {
            assert(i.choises);
            if (i.choises->choises.list.size() == 1) {
              expression = i.choises->choises.list.back().expression;
            } else {
              exceptions.printInternal("Could not resolve type of more than one choise", e->text);
            }
          }
          result += d + expressionToString(parm, expression, expectedType, sensitivityListCallback, double_brackets);
          d = ", ";
        }
        std::string left = array_type ? (double_brackets ? "{{" : "{") : "(";
        std::string right = array_type ? (double_brackets ? "}}" : "}") : ")";
        result = left + result + right;
      } else if (e->physical) {
        debug.debug("Physical");
        result = physicalToString(parm, e->physical);
      } else if (e->number) {
        debug.debug("Number");
        result = e->number->toString();
      } else if (e->string) {
        debug.debug("String");
        result = e->string->toString();
      } else if (e->identifier) {
        result = basicIdentifierToString(parm, e->identifier, &expectedType, sensitivityListCallback);
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
  std::string ExpressionParser::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* identifier,
                                                        ast::ObjectValueContainer* expected_type,
                                                        Func sensitivityListCallback) {
    std::string name = identifier->toString(true);
    ast::ObjectArguments arguments = toObjectArguments(parm, identifier->arguments);
    debug.functionStart("BasicIdentifierToString(name = " + name + (arguments.empty() ? "" : "(" + arguments.toString() + ")") +
                        ", expected_type = " + (expected_type ? expected_type->toString() : "None") + ")", true);
    if (identifier->attribute) {
      name = attributeToString(parm, name, arguments, *expected_type, identifier->attribute,
                               identifier->arguments, sensitivityListCallback);
    } else {
      auto valid =
	[&](DatabaseElement* e)
	{
	  bool result = objectWithArguments(parm, e, arguments, expected_type);
	  return result;
	};
      DatabaseResult object;
      if (parm.findOneBase(object, name, valid)) {
        if (object.object->id == ast::ObjectType::SIGNAL) {
          sensitivityListCallback(object);
        }
        bool double_brackets = false;
        name = objectToString(parm, object, identifier->arguments, sensitivityListCallback, double_brackets);
      } else {
	std::string args = arguments.toString();
	args = args.empty() ? "" : "(" + args + ")";
        throw ObjectNotFound(this, name, "Could not find definition of " + name + args +
                             " with type " + (expected_type ? expected_type->toString() : "None"), &identifier->getText());
      }
    } 
    debug.functionEnd("BasicIdentifierToString = " + name);
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::attributeToString(parameters& parm,
						  std::string name,
                                                  ast::ObjectArguments& arguments,
                                                  ast::ObjectValueContainer& expectedType,
                                                  ast::Text* attribute,
                                                  ast::AssociationList* associationList,
                                                  Func sensitivityListCallback) {
    debug.functionStart("attributeToString(name = " + name + (arguments.empty() ? "" : "(" + arguments.toString() + ")") +
			+ ")", true);
    assert(attribute);
    //auto valid = [&](DatabaseElement* e) {
    //  return e->arguments.equals(arguments);
    //};
    DatabaseResults objects;
    parm.findAll(objects, name);
    std::string attributeName = attribute->toString(true);
    DatabaseResult match;
    if (findAttributeMatch(parm, objects, match, expectedType, attributeName)) {
      assert(match.object);
      name = NameConverter::getName(parm, match, true) + "." + attributeName;
      std::string a = "";
      if (associationList) {
        std::string delimiter = "";
        bool double_brackets = false;
        for (auto& i : associationList->associationElements.list) {
          a = delimiter + expressionToString(parm, i.actualPart, match.object->type, sensitivityListCallback, double_brackets);
          delimiter = ", ";
        }
      }
      name += "(" + a + ")";
    } else {
      exceptions.printError("Could not find match for attribute \"" + attributeName +
                            "\" with expected type " + expectedType.toString(), attribute);
    }
    debug.functionEnd("attributeToString: " + name);
    return name;
  }

}


#endif

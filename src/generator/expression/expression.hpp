#ifndef _GENERATOR_EXPRESSION_PARSER_HPP
#define _GENERATOR_EXPRESSION_PARSER_HPP

#include <list>
#include <cassert>

#include "../../ast/object_type.hpp"
#include "../../exceptions/exceptions.hpp"
#include "../../debug/debug.hpp"
#include "../parameters.hpp"
#include "../name_converter.hpp"

#include "expected_type.hpp"

namespace generator {

  class ExpressionParser {

    Debug<true> m_debug;
    
    bool verbose = false;
    Exceptions m_exceptions;

    struct ReturnTypePair {
      ast::ObjectValueContainer left;
      ast::ObjectValueContainer right;
    };

    // Exception classes
  public:
    
    class ObjectNotFound {
      ExpressionParser* m_parent;
      std::string m_name;
      std::string m_message;
      ast::Text* m_text;
      std::string m_file_name;
      int m_line_number;
    public:
      ObjectNotFound(ExpressionParser* parent, std::string name,
                     std::string message, ast::Text* text,
		     const char* file_name, int line_number) {
        m_parent = parent;
        m_name = name;
        m_message = message;
        m_text = text;
	m_file_name = file_name;
	m_line_number = line_number;
      }
      const std::string getMessage() const {
	return	"[" + m_file_name + "(" + std::to_string(m_line_number) + ")] : " + m_message;
      }
      void print() {
        m_parent->m_exceptions.printError(getMessage(), m_text);
      }
      virtual const char* what() const throw() {
	std::string m = getMessage() + ": " + m_text->getCurrentLine();
	return m.c_str();
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
				  ExpectedType& expected_type,
				  Func sensitivityListCallback);
    template <typename Func>
    std::string expressionToString(parameters& parm,
				   ast::Expression* e,
				   ExpectedType& expected_type,
				   Func sensitivityListCallback,
				   std::string assignment_name = "");
    void addAssignment(std::string& x, std::string assignment, std::string& assignment_name);
    template <typename Func>
    std::string parenthisExpressionTermToString(parameters& parm,
						ast::List<ast::ElementAssociation>& parenthis_expression,
						ExpectedType& expected_type,
						Func sensitivityListCallback,
						ast::Text* text,
						std::string& assignment_name);
    template <typename Func>
    std::string expressionTermToString(parameters& parm,
				       ast::ExpressionTerm* e,
				       ExpectedType& expected_type,
				       Func sensitivityListCallback,
				       std::string assignment_name = "");
  public:
    template <typename Func>
    std::string basicIdentifierToString(parameters& parm,
					ast::BasicIdentifier* identifier,
					ExpectedType* expected_type,
					Func sensitivityListCallback);
    std::string basicIdentifierToString(parameters& parm,
					ast::BasicIdentifier* identifier,
					ExpectedType* expected_type = NULL);
  private:
    template <typename Func>
    std::string objectToString(parameters& parm,
			       DatabaseResult& object,
			       ast::AssociationList* arguments,
			       Func sensitivityListCallback);
    
    ast::ObjectValueContainer getAttributeType(parameters& parm,
					       ast::ObjectValueContainer& type,
					       std::string attributeName);
    
    bool findAttributeMatch(parameters& parm,
			    DatabaseResults& objects,
			    DatabaseResult& match,
			    ExpectedType& expected_type,
			    std::string& name);
    
    template <typename Func>
    std::string attributeToString(parameters& parm,
				  std::string name,
				  ast::ObjectArguments& arguments,
				  ExpectedType& expected_type,
				  ast::Text* attribute,
				  ast::AssociationList* associationList,
				  Func sensitivityListCallback);
    template <typename Func>
    std::string associateArgument(parameters& parm,
				  ast::ObjectInterfaceElement& interface_element,
				  int argumentNumber,
				  ast::AssociationList* l,
				  Func sensitivityListCallback); 
    template <typename Func>
    std::string parametersToString(parameters& parm,
				   ast::ObjectInterface& interface,
				   ast::AssociationList* associationList,
				   Func sensitivityListCallback);

    void toObjectArguments(parameters& parm,
			   ast::AssociationList* associationList,
			   ast::ObjectArguments& arguments);


    bool getStaticAttributeType(parameters& parm,
				std::string attributeName,
				ast::ObjectValueContainer& result);
    
    bool exists(parameters& parm,
		ReturnTypePair& pair,
		std::list<ReturnTypePair>& typePairs);

    bool objectWithArguments(parameters& parm,
			     DatabaseElement* e, ast::ObjectArguments& arguments,
			     ExpectedType* expectedReturnType = NULL,
			     ast::Text* text = NULL);

    std::string procedureCallStatementToString(parameters& parm,
					       ast::ProcedureCallStatement* p,
					       std::string& name,
					       ast::ObjectArguments& arguments);
    
  public:

    ExpressionParser(bool verbose = false) : m_debug(this) {
      this->verbose |= verbose;
    }
  
    std::string physicalToString(parameters& parm,
				 ast::Physical* physical);

    template <typename Func>
    std::string assignmentString(parameters& parm,
				 ast::Expression* e,
				 ExpectedType& expected_type,
				 Func sensitivityListCallback,
				 std::string assignment_name = "");
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ExpectedType& expected_type,
			 bool add_read_function = false);
    template <typename Func>
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ast::ObjectValue expected_type,
			 Func sensitivityListCallback);
    std::string toString(parameters& parm,
			 ast::Expression* e,
			 ast::ObjectValue expected_type);

    std::string procedureCallStatementToString(parameters& parm,
					       ast::ProcedureCallStatement* p);

    void collectAllReturnTypes(parameters& parm,
			       ast::Expression* e,
			       ExpectedType& expected_type);

    bool collectUniqueReturnType(parameters& parm,
				 ast::Expression* e,
				 ast::ObjectValueContainer& type,
				 ExpectedType* expected_type = NULL);
    
    std::string translateOperator(parameters& parm,
				  std::string& op);

    bool uniqueReturnType(parameters& parm,
			  ast::ReturnTypes& return_types,
			  ast::ObjectValueContainer& type,
			  ast::Text* text,
			  ExpectedType* expected_type = NULL);
  
  };

  template <typename Func>
  std::string ExpressionParser::assignmentString(parameters& parm,
						 ast::Expression* e,
						 ExpectedType& expected_type,
						 Func sensitivityListCallback,
						 std::string assignment_name) {
    m_debug.functionStart("assignmentString(assignment_name = " + assignment_name + ")");
    collectAllReturnTypes(parm, e, expected_type);
    std::string s = expressionToString(parm, e, expected_type, sensitivityListCallback, assignment_name);
    m_debug.functionEnd("assignmentString: " + s);
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::toString(parameters& parm,
					 ast::Expression* expr,
                                         ast::ObjectValue expected_type,
                                         Func sensitivityListCallback) {
    m_debug.functionStart("toString", false, __FILE__, __LINE__);
    ast::ObjectValueContainer e(expected_type);
    std::string s = toString(parm, expr, e, sensitivityListCallback);
    m_debug.functionEnd("toString");
    return s;
  }
    
  template<typename Func>
  void ExpressionParser::getReturnTypes(parameters& parm, std::string& name, Func valid, ast::ReturnTypes& return_types) {
    m_debug.functionStart("GetReturnTypes(name = " + name + ")");
    DatabaseResults objects;
    parm.findAll(objects, name);
    for (auto& i : objects) {
      return_types.insert(i.object->type);
    }
    m_debug.functionEnd("GetReturnTypes = " + return_types.toString());
  }
  
  
  template <typename Func>
  std::string ExpressionParser::associateArgument(parameters& parm,
						  ast::ObjectInterfaceElement& interface_element,
                                                  int argumentNumber,
                                                  ast::AssociationList* l,
                                                  Func sensitivityListCallback) { 
    m_debug.functionStart("associateArgument, interface_element = " + interface_element.toString(), true);
    std::string argument;
    ExpectedType expected_type(interface_element.m_type);
    if (interface_element.m_default_value) {
      argument = toString(parm, interface_element.m_default_value, expected_type);
    }
    if (l) {
      int associationElementNumber = 0;
      for (auto& e : l->associationElements.list) {
        try {
          std::string actualPart = expressionToString(parm, e.actualPart, expected_type, sensitivityListCallback);
          m_debug.debug("Actual part = " + actualPart);
          if (e.formalPart) {
            std::string formalPart = e.formalPart->toString(true);
            m_debug.debug("Formal part = " + formalPart + ", interface element = " + interface_element.m_name);
            if (formalPart == interface_element.m_name) {
              argument = actualPart;
              break;
            }
          } else if (associationElementNumber == argumentNumber) {
            m_debug.debug("Association number " + std::to_string(associationElementNumber) + " match");
            argument = actualPart;
            break;
          }
        } catch (ObjectNotFound& e) {
	}
        associationElementNumber++;
      }
    }
    m_debug.functionEnd("associateArgument = " + argument);
    return argument;
  }

  template <typename Func>
  std::string ExpressionParser::parametersToString(parameters& parm,
						   ast::ObjectInterface& interface,
                                                   ast::AssociationList* associationList,
                                                   Func sensitivityListCallback) { 
    m_debug.functionStart("parametersToString");
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
    for (auto& i : interface.getList()) {
      std::string argument = associateArgument(parm, i, argumentNumber, associationList, sensitivityListCallback);
      if (argument.size() == 0) {
        m_exceptions.printError("No argument associated element " + std::to_string(argumentNumber));
      }
      s += delimiter + argument;
      delimiter = ", ";
      argumentNumber++;
    };
    m_debug.functionEnd("parametersToString");
    return s;
  }

  template <typename Func>
  std::string ExpressionParser::objectToString(parameters& parm,
					       DatabaseResult& object,
                                               ast::AssociationList* arguments,
                                               Func sensitivityListCallback) {
    m_debug.functionStart("objectToString");
    assert(object.object);
    std::string name = NameConverter::getName(parm, object);
    m_debug.debug("name = " + name + ": " + object.toString());
    if (object.object->type.IsValue(ast::ObjectValue::ARRAY)) {
      if (arguments) {
        bool cast_operator = (object.object->id == ast::ObjectType::TYPE);
        if (cast_operator) {
          assert(arguments->associationElements.list.size() == 1);
          ast::Expression* arg = arguments->associationElements.list.front().actualPart;
	  ExpectedType expected_type(object.object->type);
	  name += "(" + expressionToString(parm, arg, expected_type, sensitivityListCallback) + ")";
        } else {
          std::string parameters;
          std::string delimiter = "";
          ast::ObjectValueContainer::Array& args = object.object->type.GetArguments();
          assert(args.size() == arguments->associationElements.list.size());
          auto it = args.begin();
          for (auto& i : arguments->associationElements.list) {
	    ExpectedType expected_type(*it);
	    parameters += delimiter + expressionToString(parm, i.actualPart, expected_type, sensitivityListCallback);
            it++;
            delimiter = "][";
          }
          name += "[" + parameters + "]";
        }
      }
    } else if (arguments) {
      std::string parameters = parametersToString(parm, object.object->interface, arguments, sensitivityListCallback);
      name += "(" + parameters + ")";
    } else if (object.object->id == ast::ObjectType::FUNCTION ||
               object.object->id == ast::ObjectType::PROCEDURE) {
      name += "()";
    }
    m_debug.functionEnd("objectToString = " + name);
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::operationToString(parameters& parm,
						  ast::Expression* e,
                                                  ExpectedType& expected_type,
                                                  Func sensitivityListCallback) {
    m_debug.functionStart("operationToString(expected_type = " + expected_type.toString() + ")", false, __FILE__, __LINE__);
    std::string result;
    std::list<ReturnTypePair> typePairs;
    ast::ReturnTypes t;
    for (const ast::ObjectValueContainer& i : e->term->returnTypes.getList()) {
      for (const ast::ObjectValueContainer& j : e->expression->returnTypes.getList()) {
        t.clear();
        operatorReturnTypes(parm, e->op->op, i, j, t);
        for (const ast::ObjectValueContainer& x : t.getList()) {
          if (expected_type.equals(x)) {
            ReturnTypePair pair = {i, j};
            if (!exists(parm, pair, typePairs)) {
              typePairs.push_back(pair);
            }
          }
        }
      }
    }
    if (typePairs.size() != 1) {
      m_exceptions.printError("Cound not resolve expected type " + expected_type.toString(), e->text);
      if (typePairs.empty()) {
        std::cerr << "Did not find anything" << std::endl;
      } else {
        std::cerr << "Found the following types:" << std::endl;
        for (auto& i : typePairs) {
          std::cerr << i.left.toString() + " " + e->op->op + " " + i.right.toString() + " = " + expected_type.toString() << std::endl;
        }
      }
    } else {
      ExpectedType expected_type_left(typePairs.back().left);
      std::string term = expressionTermToString(parm, e->term, expected_type_left, sensitivityListCallback);
      ExpectedType expected_type_right(typePairs.back().right);
      std::string expr = expressionToString(parm, e->expression, expected_type_right, sensitivityListCallback);
      std::string op;
      op = translateOperator(parm, e->op->op);
      result = term + " " + op + " " + expr;
    }
    m_debug.functionEnd("operationToString");
    return result;
  }
  
  template <typename Func>
  std::string ExpressionParser::expressionToString(parameters& parm,
						   ast::Expression* e,
						   ExpectedType& expected_type,
                                                   Func sensitivityListCallback,
                                                   std::string assignment_name) {
    m_debug.functionStart("expressionToString(expected_type = " + expected_type.toString() + ")", false, __FILE__, __LINE__);
    assert(e);
    std::string result;
    if (e->unaryOperator) {
      std::string op;
      std::string expr = expressionToString(parm, e->expression, expected_type, sensitivityListCallback);
      switch (e->unaryOperator->op) {
      case ::ast::UnaryOperator::NOT: {op = "!"; break;}
      case ::ast::UnaryOperator::MINUS: {op = "-"; break;}
      default: {assert (false);}
      }
      result = op + expr;
    } else if (e->op) {
      result = operationToString(parm, e, expected_type, sensitivityListCallback);
    } else {
      result = expressionTermToString(parm, e->term, expected_type, sensitivityListCallback, assignment_name);
    }
    m_debug.functionEnd("expressionToString = " + result);
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::parenthisExpressionTermToString(parameters& parm,
								ast::List<ast::ElementAssociation>& parenthis_expression,
								ExpectedType& expected_type,
								Func sensitivityListCallback,
								ast::Text* text,
								std::string& assignment_name) {
    m_debug.functionStart("parenthisExpressionTermToString(assignment_name = " + assignment_name + ", expected_type = " + expected_type.toString() + ")", false, __FILE__, __LINE__, text);
    std::string result;
    if (!expected_type.isUnique()) {
      m_exceptions.printError("Could not determine parenthis type since expected type is not unique. Expected type is: " + expected_type.toString(), text);
    } else {
      for (ast::ElementAssociation& i : parenthis_expression.list) {
	if (i.choises) {
	  for (ast::Choise& j : i.choises->choises.list) {
	    if (j.others) {
	      addAssignment(result, ".setOthers(" + expressionToString(parm, i.expression, expected_type, sensitivityListCallback, assignment_name) + ")", assignment_name);
	    }
	  }
	}
      }
      int index = 0;
      for (ast::ElementAssociation& i : parenthis_expression.list) {
	std::string assignment_name_next_hierarchy = (assignment_name.empty() ? "" : assignment_name + ".VAL(" + std::to_string(index) + ")");
	ast::Expression* expression = i.expression;
	if (!expression) {
	  assert(i.choises);
	  if (i.choises->choises.list.size() == 1) {
	    expression = i.choises->choises.list.back().expression;
	    std::string x = expressionToString(parm, expression, expected_type, sensitivityListCallback, assignment_name_next_hierarchy);
	    if (expected_type.isArray()) {
	      ast::ObjectValueContainer return_type;
	      bool return_type_found = collectUniqueReturnType(parm, expression, return_type, &expected_type);
	      if (return_type_found && return_type.isArray()) {
		addAssignment(result, ".setArray(" + x + ")", assignment_name);
	      } else {
		addAssignment(result,  ".setIndex(" + std::to_string(index) + ", " + x + ")", assignment_name);
	      }
	    } else {
	      result = "(" + x + ")";
	    }
	  } else {
	    m_exceptions.printInternal("Could not resolve type of more than one choise", text);
	  }
	} else {
	  for (ast::Choise& j : i.choises->choises.list) {
	    if (j.expression) {
	      std::string element = expressionToString(parm, j.expression, expected_type, sensitivityListCallback, assignment_name_next_hierarchy);
	      std::string x = expressionToString(parm, expression, expected_type, sensitivityListCallback, assignment_name_next_hierarchy);
	      addAssignment(result, ".setElement(" + element + ", " + x + ")", assignment_name);
	    } else if (j.discrete_range) {
	      std::string left = expressionToString(parm, j.discrete_range->left, expected_type, sensitivityListCallback);
	      std::string right = expressionToString(parm, j.discrete_range->right, expected_type, sensitivityListCallback);
	      std::string x = expressionToString(parm, expression, expected_type, sensitivityListCallback, assignment_name_next_hierarchy);
	      addAssignment(result, ".setRange(" + left + ", " + right + ", " + x + ")", assignment_name);
	    }
	  }
	}
	index++;
      }
    }
    m_debug.functionEnd("parenthisExpressionTermToString: " + result);
    return result;
  }
  
  template <typename Func>
  std::string ExpressionParser::expressionTermToString(parameters& parm,
						       ast::ExpressionTerm* e,
						       ExpectedType& expected_type,
                                                       Func sensitivityListCallback,
                                                       std::string assignment_name) {
    std::string result = "";
    if (e) {
      m_debug.functionStart("expressionTermToString(expected_type = " + expected_type.toString() + ")", false, __FILE__, __LINE__, e->text);
      if (!e->parenthis.list.empty()) {
	if (expected_type.isArray()) {
	  ExpectedType expected_subtype;
	  expected_subtype.subtype(expected_type);
	  result = parenthisExpressionTermToString(parm,
						   e->parenthis,
						   expected_subtype,
						   sensitivityListCallback,
						   e->text,
						   assignment_name);
	} else {
	  result = parenthisExpressionTermToString(parm,
						   e->parenthis,
						   expected_type,
						   sensitivityListCallback,
						   e->text,
						   assignment_name);
	}
      } else if (e->physical) {
        m_debug.debug("Physical");
        result = physicalToString(parm, e->physical);
      } else if (e->number) {
        m_debug.debug("Number");
        result = e->number->toString();
      } else if (e->string) {
        m_debug.debug("String");
        result = e->string->toString();
      } else if (e->identifier) {
        result = basicIdentifierToString(parm, e->identifier, &expected_type, sensitivityListCallback);
      } else if (e->character) {
        m_debug.debug("Character");
        result = e->character->toString();
      } else {
        assert(false);
      }
      m_debug.functionEnd("expressionTermToString");
    }
    return result;
  }

  template <typename Func>
  std::string ExpressionParser::basicIdentifierToString(parameters& parm, ast::BasicIdentifier* identifier,
							ExpectedType* expected_type,
                                                        Func sensitivityListCallback) {
    std::string name = identifier->toString(true);
    ast::ObjectArguments arguments;
    toObjectArguments(parm, identifier->arguments, arguments);
    m_debug.functionStart("BasicIdentifierToString(name = " + name + (arguments.empty() ? "" : "(" + arguments.toString() + ")") +
			  ", expected_type = " + (expected_type ? expected_type->toString() : "None") + ")", false, __FILE__, __LINE__);
    if (identifier->attribute) {
      name = attributeToString(parm, name, arguments, *expected_type, identifier->attribute,
                               identifier->arguments, sensitivityListCallback);
    } else {
      auto valid =
	[&](DatabaseElement* e)
	{
	  bool result = objectWithArguments(parm, e, arguments, expected_type, &identifier->getText());
	  return result;
	};
      DatabaseResult object;
      if (parm.findOneBase(object, name, valid)) {
	std::string name_extension;
	if (object.object->id == ast::ObjectType::SIGNAL || object.object->id == ast::ObjectType::PORT) {
	  sensitivityListCallback(object, name_extension);
        }
        name = objectToString(parm, object, identifier->arguments, sensitivityListCallback) + name_extension;
      } else {
	std::string args = arguments.toString();
	args = args.empty() ? "" : "(" + args + ")";
        throw ObjectNotFound(this, name, "Could not find definition of " + name + args +
                             " with type " + (expected_type ? expected_type->toString() : "None"),
			     &identifier->getText(), __FILE__, __LINE__);
      }
    } 
    m_debug.functionEnd("BasicIdentifierToString = " + name);
    return name;
  }

  template <typename Func>
  std::string ExpressionParser::attributeToString(parameters& parm,
						  std::string name,
                                                  ast::ObjectArguments& arguments,
						  ExpectedType& expected_type,
                                                  ast::Text* attribute,
                                                  ast::AssociationList* associationList,
                                                  Func sensitivityListCallback) {
    m_debug.functionStart("attributeToString(name = " + name + (arguments.empty() ? "" : "(" + arguments.toString() + ")") +
			+ ")", true);
    assert(attribute);
    //auto valid = [&](DatabaseElement* e) {
    //  return e->arguments.equals(arguments);
    //};
    DatabaseResults objects;
    parm.findAll(objects, name);
    std::string attributeName = attribute->toString(true);
    DatabaseResult match;
    if (findAttributeMatch(parm, objects, match, expected_type, attributeName)) {
      assert(match.object);
      name = NameConverter::getName(parm, match, true) + "." + attributeName;
      std::string a = "";
      if (associationList) {
        std::string delimiter = "";
        for (auto& i : associationList->associationElements.list) {
	  ExpectedType expected_type(match.object->type);
	  a = delimiter + expressionToString(parm, i.actualPart, expected_type, sensitivityListCallback);
          delimiter = ", ";
        }
      }
      name += "(" + a + ")";
    } else {
      m_exceptions.printError("Could not find match for attribute \"" + attributeName +
                            "\" with expected type " + expected_type.toString(), attribute);
    }
    m_debug.functionEnd("attributeToString: " + name);
    return name;
  }

}


#endif

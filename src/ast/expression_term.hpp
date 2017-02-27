
#ifndef AST_EXPRESSION_TERM_H_
#define AST_EXPRESSION_TERM_H_

#include "number.hpp"
#include "string.hpp"
#include "basic_identifier.hpp"
#include "physical.hpp"
#include "character.hpp"
#include "procedure_call_statement.hpp"

namespace ast {

  class ProcedureCallStatement;
  
  class ExpressionTerm {
    
  public:

    Physical* physical;
    Number* number;
    String* text;
    BasicIdentifier* identifier;
    Character* character;
    ProcedureCallStatement* procedureCall;
    
  };

}

#endif

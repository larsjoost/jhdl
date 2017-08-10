
#ifndef AST_CASE_STATEMENT_HPP_
#define AST_CASE_STATEMENT_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "case_statement_alternative.hpp"
#include "association_list.hpp"

namespace ast {

  class CaseStatementAlternative;
  
  class CaseStatement {
    
  public:
    
    SimpleIdentifier* identifier;
    AssociationList* arguments = NULL; 
    List<CaseStatementAlternative> alternatives;
    
  };

}

#endif


#ifndef AST_WAIT_STATEMENT_H_
#define AST_WAIT_STATEMENT_H_

#include "physical.hpp"
#include "text.hpp"

namespace ast {

  
  class WaitStatement {
    
  public:
    Text* waitText;
    Physical* physical;

  };

}

#endif

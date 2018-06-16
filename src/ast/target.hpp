
#ifndef AST_TARGET_HPP_
#define AST_TARGET_HPP_

#include "target_argument.hpp"
#include "simple_identifier.hpp"
#include "list.hpp"

namespace ast {
    
  class Target {
    
  public:

    SimpleIdentifier* identifier;
    Target* element;
    List<TargetArgument> arguments;
    
  };

}

#endif

  
#ifndef AST_FILE_DECLARATION_H_
#define AST_FILE_DECLARATION_H_

#include "simple_identifier.hpp"
#include "string.hpp"

namespace ast {
    
  class FileDeclaration {
    
  public:

    // file <handle> : <type> open <Direction> is <filename>;
    
    SimpleIdentifier* handle;
    SimpleIdentifier* type;
    SimpleIdentifier* direction;
    String* filename;
    
  };

}

#endif

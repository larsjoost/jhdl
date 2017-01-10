
#ifndef PARSER_BASE_H_
#define PARSER_BASE_H_

#include "../ast/scanner.hpp"

namespace parser {

  class ParseNotImplemented {};
  
  class Base {
  public:
    virtual void parse(ast::Scanner* s) {
      throw ParseNotImplemented();
    }
    
  };

}

#endif

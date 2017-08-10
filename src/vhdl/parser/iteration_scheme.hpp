
#ifndef VHDL_PARSER_ITERATION_SCHEME_H_
#define VHDL_PARSER_ITERATION_SCHEME_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/iteration_scheme.hpp"

namespace vhdl {
  namespace parser {

    class IterationScheme :
      public ::ast::IterationScheme  {

    public:

      IterationScheme* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

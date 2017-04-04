
#ifndef VHDL_PARSER_FUNCTIONBODY_H_
#define VHDL_PARSER_FUNCTIONBODY_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/function_body.hpp"

namespace vhdl {
  namespace parser {

    class FunctionBody :
      public ::ast::FunctionBody  {

    public:

      FunctionBody* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
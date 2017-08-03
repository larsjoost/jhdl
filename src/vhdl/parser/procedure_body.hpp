
#ifndef VHDL_PARSER_PROCEDUREBODY_H_
#define VHDL_PARSER_PROCEDUREBODY_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/function_body.hpp"

namespace vhdl {
  namespace parser {

    class ProcedureBody :
      public ::ast::FunctionBody  {

    public:

      ProcedureBody* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

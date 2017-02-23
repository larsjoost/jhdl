
#ifndef VHDL_PARSER_UNARY_OPERATOR_H_
#define VHDL_PARSER_UNARY_OPERATOR_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/unary_operator.hpp"

namespace vhdl {
  namespace parser {

    class UnaryOperator :
      public ::ast::UnaryOperator  {

    public:

      UnaryOperator* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif


#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionOperator* ExpressionOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      if (scanner->accept("&")) {op = "&";}
      return this;
    }

  }
}


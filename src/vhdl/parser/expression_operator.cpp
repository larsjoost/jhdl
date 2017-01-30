
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionOperator* ExpressionOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      static const int SIZE = 2;
      static const char* o[SIZE] = {"&", "+"};
      for (int i=0; i<SIZE; i++) {
        if (scanner->optional(o[i])) {
          op = o[i];
          break;
        }
      }
      return this;
    }

  }
}


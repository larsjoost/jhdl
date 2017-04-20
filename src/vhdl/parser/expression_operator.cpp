
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionOperator* ExpressionOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      static const int SIZE = 7;
      static const char* c[SIZE] = {"/=", ">=", "<=", "&", "+", "-", "="};
      for (int i=0; i<SIZE; i++) {
        if (scanner->optional(c[i])) {
          op = std::string(c[i]);
          return this;
        }
      }
      return this;
    }

  }
}



#include "../../ast/scanner.hpp"
#include "../../ast/expression_operator.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionOperator* ExpressionOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      {
        static const int SIZE = 3;
        static const ::ast::ExpressionOperator::operators o[SIZE] = {
          ::ast::ExpressionOperator::NOT_EQUAL,
          ::ast::ExpressionOperator::LARGER_OR_EQUAL,
          ::ast::ExpressionOperator::SMALLER_OR_EQUAL};
        static const char* c[SIZE] = {"/=", ">=", "<="};
        for (int i=0; i<SIZE; i++) {
          if (scanner->optional(c[i])) {
            op = o[i];
            return this;
          }
        }
      }
      {
        static const int SIZE = 4;
        static const ::ast::ExpressionOperator::operators o[SIZE] = {
          ::ast::ExpressionOperator::CONCAT,
          ::ast::ExpressionOperator::ADD,
          ::ast::ExpressionOperator::SUB,
          ::ast::ExpressionOperator::EQUAL};
        static const char* c[SIZE] = {"&", "+", "-", "="};
        for (int i=0; i<SIZE; i++) {
          if (scanner->optional(c[i])) {
            op = o[i];
            return this;
          }
        }
      }
      return this;
    }

  }
}


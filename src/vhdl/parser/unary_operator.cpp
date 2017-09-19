
#include "../../ast/scanner.hpp"
#include "../../ast/unary_operator.hpp"
#include "../scanner/scanner.hpp"
#include "unary_operator.hpp"

namespace vhdl {
  namespace parser {
  
    UnaryOperator* UnaryOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      {
        static const int SIZE = 1;
        static const ::ast::UnaryOperator::operators o[SIZE] = {
          ::ast::UnaryOperator::NOT};
        static scanner::Scanner::Keyword c[SIZE] = {scanner::Scanner::Keyword::VHDL_NOT};
        for (int i=0; i<SIZE; i++) {
          if (scanner->optional(c[i])) {
            op = o[i];
            return this;
          }
        }
      }
      {
        static const int SIZE = 2;
        static const ::ast::UnaryOperator::operators o[SIZE] = {
          ::ast::UnaryOperator::MINUS};
        static const char* c[SIZE] = {"-", "+"};
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

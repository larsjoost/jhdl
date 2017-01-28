#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "number_type.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    NumberType* NumberType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_RANGE);
      left = scanner->expect<Expression>();
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_TO);
      right = scanner->expect<Expression>();
      return this;
    }

  }
}


#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "expression.hpp"
#include "number.hpp"

namespace vhdl {
  namespace parser {
  
    Expression* Expression::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      number = scanner->expect<Number>();
      return this;
    }

  }
}


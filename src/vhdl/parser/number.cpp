#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "number.hpp"
#include "object_type.hpp"

namespace vhdl {
  namespace parser {
  
    Number* Number::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ::ast::Text* v;
      if (v = scanner->optional(::ast::TOKEN_INTEGER)) {
        value = *v;
        type = ast::INTEGER;
      } else if (v = scanner->optional(::ast::TOKEN_REAL)) {
        value = *v;
        type = ast::REAL;
      }
      return this;
    }
    
  }
}


#include "../../ast/scanner.hpp"
#include "../../ast/object_type.hpp"
#include "../scanner/scanner.hpp"
#include "number.hpp"

namespace vhdl {
  namespace parser {
  
    Number* Number::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ::ast::Text* v;
      if (v = scanner->optional(::ast::TOKEN_INTEGER)) {
        value = *v;
        type = ast::ObjectValue::INTEGER;
      } else if (v = scanner->optional(::ast::TOKEN_REAL)) {
        value = *v;
        type = ast::ObjectValue::REAL;
      }
      return this;
    }
    
  }
}


#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_declaration.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Declaration* Declaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      type = scanner->expect<TypeDeclaration>();
      return this;
    }

  }
}


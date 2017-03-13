#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp" 
#include "list.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    void BasicIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ::ast::Text* name = scanner->accept(::ast::TOKEN_IDENTIFIER);
      text = *name;
      if (scanner->optional("'")) {
        attribute = scanner->expect(::ast::TOKEN_IDENTIFIER);
        if (scanner->optional("(")) {
          arguments = scanner->optional<List<Expression, ::ast::Expression, ','>>();
          scanner->expect(")");
        }
      }
    }

  }
}


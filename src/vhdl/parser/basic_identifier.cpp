#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp" 
#include "association_list.hpp" 
#include "list.hpp"
#include "expression.hpp"
#include "range_type.hpp"

namespace vhdl {
  namespace parser {
  
    void BasicIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = *(scanner->accept(::ast::TOKEN_IDENTIFIER));
      bool expect_parenthis = true;
      if (scanner->optional("'")) {
        if (scanner->optional(scanner::Scanner::VHDL_RANGE)) {
          range_attribute = true;
        } else if (scanner->optional("(")) {
          type_select = scanner->expect<Expression>();
          scanner->expect(")");
          expect_parenthis = false;
        } else {
          attribute = scanner->expect(::ast::TOKEN_IDENTIFIER);
        }
      } 
      if (expect_parenthis && scanner->optional("(")) {
        (
         (range = scanner->optional<RangeType>()) ||
         (arguments = scanner->optional<AssociationList>()));
        scanner->expect(")");
      }
    }

  }
}


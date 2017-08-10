#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp" 
#include "association_list.hpp" 
#include "list.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    void BasicIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = *(scanner->accept(::ast::TOKEN_IDENTIFIER));
      if (scanner->optional("'")) {
        if (scanner->optional(scanner::Scanner::VHDL_RANGE)) {
          range_attribute = true;
        } else {
          attribute = scanner->expect(::ast::TOKEN_IDENTIFIER);
        }
      } 
      if (scanner->optional("(")) {
        arguments = scanner->optional<AssociationList>();
        scanner->expect(")");
      }
    }

  }
}


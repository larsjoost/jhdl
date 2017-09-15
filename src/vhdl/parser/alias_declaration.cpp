#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "alias_declaration.hpp"
#include "simple_identifier.hpp"
#include "subtype_indication.hpp"
#include "list.hpp"
#include "return_statement.hpp"

namespace vhdl {
  namespace parser {
  
    AliasDeclaration* AliasDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ALIAS);
      designator = scanner->expect<SimpleIdentifier>();
      if (scanner->optional(":")) {
        type = scanner->expect<SubtypeIndication>();
      }
      scanner->expect(scanner::Scanner::VHDL_IS);
      name = scanner->expect<SimpleIdentifier>();
      if (scanner->optional("[")) {
        arguments = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>>();
        returnStatement = scanner->optional<ReturnStatement>();
        scanner->expect("]");
      }
      return this;
    }

  }
}


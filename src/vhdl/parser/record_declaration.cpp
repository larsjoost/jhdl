#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "record_declaration.hpp"
#include "element_declaration_list.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    RecordDeclaration* RecordDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      declaration = scanner->expect<ElementDeclarationList, RecordDeclaration>();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_RECORD);
      scanner->optional<SimpleIdentifier>();
      return this;
    }

  }
}


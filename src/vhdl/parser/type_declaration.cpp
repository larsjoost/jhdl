#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_declaration.hpp"
#include "type_definition.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "interface_list.hpp"
#include "record_declaration.hpp"

namespace vhdl {
  namespace parser {
  
    TypeDeclaration* TypeDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_TYPE);
      identifier = scanner->expect<SimpleIdentifier, TypeDeclaration>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      if (scanner->optional(scanner::Scanner::VHDL_ACCESS)) {
        accessType = scanner->expect<SimpleIdentifier, TypeDeclaration>();
      } else if (scanner->optional(scanner::Scanner::VHDL_FILE)) {
        scanner->expect(scanner::Scanner::VHDL_OF);
        fileType = scanner->expect<SimpleIdentifier, TypeDeclaration>();
      } else if (scanner->optional(scanner::Scanner::VHDL_RECORD)) {
        record = scanner->expect<RecordDeclaration, TypeDeclaration>();
      } else {
        typeDefinition = scanner->expect<TypeDefinition, TypeDeclaration>();
      }
      return this;
    }

  }
}


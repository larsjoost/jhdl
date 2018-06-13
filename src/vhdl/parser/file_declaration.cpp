#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "file_declaration.hpp"
#include "string.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    FileDeclaration* FileDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_FILE);
      handle = scanner->expect<SimpleIdentifier, FileDeclaration>();
      scanner->expect(":");
      type = scanner->expect<SimpleIdentifier, FileDeclaration>();
      scanner->expect(scanner::Scanner::VHDL_OPEN);
      direction = scanner->expect<SimpleIdentifier, FileDeclaration>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      filename = scanner->expect<String, FileDeclaration>();
      return this;
    }

  }
}


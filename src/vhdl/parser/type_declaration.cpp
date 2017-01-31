#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_declaration.hpp"
#include "type_definition.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    TypeDeclaration* TypeDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_TYPE);
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      identifier = i->text;
      scanner->accept(scanner::Scanner::VHDL_IS);
      typeDefinition = scanner->expect<TypeDefinition>();
      scanner->expect(";");
      return this;
    }

  }
}


#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "component_instance.hpp"
#include "basic_identifier.hpp"
#include "association_list.hpp"
#include "basic_identifier_list.hpp"

namespace vhdl {
  namespace parser {
  
    ComponentInstance* ComponentInstance::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      instanceName = scanner->accept<BasicIdentifier>();
      scanner->accept(":");
      scanner->optional(scanner::Scanner::VHDL_ENTITY);
      componentName = scanner->accept<BasicIdentifierList<PunctuationSeparation>>();
      if (scanner->optional("(")) {
        architectureName = scanner->expect<BasicIdentifier>();
        scanner->expect(")");
      }
      if (scanner->optional(scanner::Scanner::VHDL_GENERIC)) {
        scanner->expect(scanner::Scanner::VHDL_MAP);
        scanner->expect("(");
        generics = scanner->expect<AssociationList>();
        scanner->expect(")");
      }
      if (scanner->optional(scanner::Scanner::VHDL_PORT)) {
        scanner->expect(scanner::Scanner::VHDL_MAP);
        scanner->expect("(");
        ports = scanner->expect<AssociationList>();
        scanner->expect(")");
      }
      return this;
    }

  }
}

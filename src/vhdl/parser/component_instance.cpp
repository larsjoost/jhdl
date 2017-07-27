#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "component_instance.hpp"
#include "simple_identifier.hpp"
#include "association_list.hpp"
#include "list.hpp"

namespace vhdl {
  namespace parser {
  
    ComponentInstance* ComponentInstance::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      instanceName = scanner->accept<SimpleIdentifier>();
      scanner->accept(":");
      scanner->optional(scanner::Scanner::VHDL_ENTITY);
      componentName = scanner->expect<SimpleIdentifier>();
      if (scanner->optional(".")) {
        libraryName = componentName;
        componentName = scanner->expect<SimpleIdentifier>();
      }
      if (scanner->optional("(")) {
        architectureName = scanner->expect<SimpleIdentifier>();
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

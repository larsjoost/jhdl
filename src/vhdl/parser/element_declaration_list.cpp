
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "element_declaration_list.hpp"
#include "element_declaration.hpp"

namespace vhdl {
  namespace parser {
  
    ElementDeclarationList* ElementDeclarationList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      while (element_declaration.add(scanner->optional<ElementDeclaration>())) {
        scanner->expect(";");
      };
      return this;
    }

  }
}


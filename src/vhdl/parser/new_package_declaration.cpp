
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "new_package_declaration.hpp"
#include "simple_identifier.hpp"
#include "association_list.hpp"

namespace vhdl {
  namespace parser {
  
    NewPackageDeclaration* NewPackageDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      m_debug.functionStart("parse");
      library = scanner->expect<SimpleIdentifier, NewPackageDeclaration>();
      scanner->expect(".");
      package = scanner->expect<SimpleIdentifier, NewPackageDeclaration>();
      scanner->expect(scanner::Scanner::VHDL_GENERIC);
      scanner->expect(scanner::Scanner::VHDL_MAP);
      scanner->expect("(");
      generics = scanner->expect<AssociationList, NewPackageDeclaration>();
      scanner->expect(")");
      scanner->expect(";");
      m_debug.functionEnd("parse");
      return this;
    }
    
  }
}


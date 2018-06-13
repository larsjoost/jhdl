#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "library_clause.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    LibraryClause* LibraryClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_LIBRARY);
      BasicIdentifier* i = scanner->expect<BasicIdentifier, LibraryClause>();
      scanner->expect(";");
      return this;
    }
    
  }
}

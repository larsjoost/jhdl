#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "library_clause.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    LibraryClause* LibraryClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_LIBRARY);
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }
    
  }
}

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "use_clause.hpp"
#include "list.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    UseClause* UseClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_USE);
      library = scanner->expect<SimpleIdentifier>();
      scanner->expect(".");
      package = scanner->expect<SimpleIdentifier>();
      scanner->expect(".");
      identifier = scanner->expect<SimpleIdentifier>();
      scanner->expect(";");
      return this;
    }
    
  }
}


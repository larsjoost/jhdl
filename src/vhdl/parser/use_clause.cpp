#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "use_clause.hpp"
#include "list.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    UseClause* UseClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_USE);
      list = scanner->expect<List<BasicIdentifier, ::ast::BasicIdentifier, '.'>>();
      scanner->expect(";");
      return this;
    }
    
  }
}


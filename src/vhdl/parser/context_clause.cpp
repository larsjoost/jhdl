
#include "../../ast/scanner.hpp"
#include "use_clause.hpp"
#include "context_clause.hpp"
#include "library_clause.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {
  
    ContextClause* ContextClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->optional<LibraryClause>();
      useClauses.add(scanner->optional<UseClause>());
      return this;
    }
    
  }
}



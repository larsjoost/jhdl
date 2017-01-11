
#include "use_clause.hpp"
#include "context_clause.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {
  
    ContextClause* ContextClause::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      useClauses.add(scanner->optional<UseClause>());
      return this;
    }
    
  }
}



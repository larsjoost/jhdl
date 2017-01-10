
#include "use_clause.hpp"
#include "context_clause.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {
  
    void ContextClause::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      useClauses.add(scanner->accept<UseClause>());
    }
    
  }
}



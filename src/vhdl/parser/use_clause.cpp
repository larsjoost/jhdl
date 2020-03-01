



#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "use_clause.hpp"
#include "list.hpp"
#include "simple_identifier.hpp"
#include "string.hpp"

namespace vhdl {
  namespace parser {
  
    UseClause* UseClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_USE);
      library = scanner->expect<SimpleIdentifier, UseClause>();
      scanner->expect(".");
      package = scanner->expect<SimpleIdentifier, UseClause>();
      scanner->expect(".");
      if (!(all = scanner->optional(scanner::Scanner::VHDL_ALL))) {
	if (!(string_identifier = scanner->optional<String>())) {
          identifier = scanner->expect<SimpleIdentifier, UseClause>();
	}
      }
      scanner->expect(";");
      return this;
    }
    
  }
}


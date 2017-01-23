#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "use_clause.hpp"
#include "basic_identifier_list.hpp"

namespace vhdl {
  namespace parser {
  
    UseClause* UseClause::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_USE);
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifierList* i;
      i = scanner->expect<BasicIdentifierList>();
      list = i->textList;
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }
    
  }
}


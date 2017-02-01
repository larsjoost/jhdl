#include "../../ast/scanner.hpp"
#include "../../ast/conditional_statement.hpp"
#include "../scanner/scanner.hpp"
#include "forloop_statement.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "sequential_statement.hpp"
#include "range_type.hpp"

namespace vhdl {
  namespace parser {
  
    ForLoopStatement* ForLoopStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_FOR);
      identifier = scanner->expect<BasicIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IN);
      range = scanner->expect<RangeType>();
      scanner->expect(scanner::Scanner::VHDL_LOOP);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_LOOP);
      scanner->expect(";");
      return this;
    }

  }
}

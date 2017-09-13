#include "../../ast/scanner.hpp"
#include "../../ast/conditional_statement.hpp"
#include "../scanner/scanner.hpp"
#include "whileloop_statement.hpp"
#include "expression.hpp"
#include "sequential_statement.hpp"

namespace vhdl {
  namespace parser {
  
    WhileLoopStatement* WhileLoopStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_WHILE);
      condition = scanner->expect<Expression>();
      scanner->expect(scanner::Scanner::VHDL_LOOP);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_LOOP);
      return this;
    }

  }
}

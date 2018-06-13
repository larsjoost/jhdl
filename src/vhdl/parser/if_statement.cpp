#include "../../ast/scanner.hpp"
#include "../../ast/conditional_statement.hpp"
#include "../scanner/scanner.hpp"
#include "if_statement.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "sequential_statement.hpp"

namespace vhdl {
  namespace parser {
  
    IfStatement* IfStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_IF);
      ::ast::ConditionalStatement *c = new ::ast::ConditionalStatement();
      c->condition = scanner->expect<Expression, IfStatement>();
      scanner->expect(scanner::Scanner::VHDL_THEN);
      while (c->sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      conditionalStatements.add(c);
      while (scanner->optional(scanner::Scanner::VHDL_ELSIF)) {
	c = new ::ast::ConditionalStatement();
	c->condition = scanner->expect<Expression, IfStatement>();
	scanner->expect(scanner::Scanner::VHDL_THEN);
	while (c->sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
	conditionalStatements.add(c);
      }
      if (scanner->optional(scanner::Scanner::VHDL_ELSE)) {
	c = new ::ast::ConditionalStatement();
	while (c->sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
	conditionalStatements.add(c);
      }
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_IF);
      return this;
    }

  }
}

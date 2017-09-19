#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression.hpp"
#include "return_statement.hpp"

namespace vhdl {
  namespace parser {
  
    ReturnStatement* ReturnStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_RETURN);
      value = scanner->optional<Expression>();
      return this;
    }

  }
}

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "wait_statement.hpp"
#include "physical.hpp"

namespace vhdl {
  namespace parser {
  
    WaitStatement* WaitStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      waitText = scanner->accept(scanner::Scanner::VHDL_WAIT);
      scanner->expect(scanner::Scanner::VHDL_FOR);
      physical = scanner->expect<Physical, WaitStatement>();
      return this;
    }

  }
}

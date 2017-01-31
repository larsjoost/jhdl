

#include "../../ast/scanner.hpp"
#include "../../ast/sequential_statement.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "process.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Process* Process::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_PROCESS);
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {};
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_PROCESS);
      scanner->expect(";");
      return this;
    }
   
  }
}


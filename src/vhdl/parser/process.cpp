

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "process.hpp"
#include "basic_identifier.hpp"
#include "procedure_call_statement.hpp"

namespace vhdl {
  namespace parser {
  
    Process* Process::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_PROCESS);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_IS);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      parseBody(scanner);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->skipWhiteSpace();
      if (scanner->optional(scanner::Scanner::VHDL_PROCESS)) {
        scanner->skipWhiteSpace();
      };
      scanner->expect(";");
      return this;
    }

    void Process::parseBody(::ast::Scanner<scanner::Scanner>* scanner) {
      bool match;
      do {
        match = false;
        match |= sequentialStatements.add(scanner->optional<ProcedureCallStatement>());
      } while (match);
    }
    
  }
}


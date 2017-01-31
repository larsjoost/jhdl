

#include "../../ast/scanner.hpp"
#include "../../ast/sequential_statement.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "process.hpp"
#include "basic_identifier.hpp"
#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "report_statement.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Process* Process::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_PROCESS);
      scanner->expect(scanner::Scanner::VHDL_IS);
      parseDeclarations(scanner);
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      parseBody(scanner);
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_PROCESS);
      scanner->expect(";");
      return this;
    }

    bool Process::add(::ast::ProcedureCallStatement* p) {
      if (p) {
        ::ast::SequentialStatement* s = new ::ast::SequentialStatement();
        s->procedureCallStatement = p;
        sequentialStatements.add(s);
        return true;
      }
      return false;
    }
    
    bool Process::add(::ast::VariableAssignment* p) {
      if (p) {
        ::ast::SequentialStatement* s = new ::ast::SequentialStatement();
        s->variableAssignment = p;
        sequentialStatements.add(s);
        return true;
      }
      return false;
    }

    bool Process::add(::ast::ReportStatement* p) {
      if (p) {
        ::ast::SequentialStatement* s = new ::ast::SequentialStatement();
        s->reportStatement = p;
        sequentialStatements.add(s);
        return true;
      }
      return false;
    }

    void Process::parseDeclarations(::ast::Scanner<scanner::Scanner>* scanner) {
      bool match;
      do {
        match = false;
        match |= declarations.add(scanner->optional<Declaration>());
      } while (match);
    }
    
    void Process::parseBody(::ast::Scanner<scanner::Scanner>* scanner) {
      bool match;
      do {
        match = false;
        match |= add(scanner->optional<VariableAssignment>());
        match |= add(scanner->optional<ReportStatement>());
        match |= add(scanner->optional<ProcedureCallStatement>());
      } while (match);
    }
    
  }
}


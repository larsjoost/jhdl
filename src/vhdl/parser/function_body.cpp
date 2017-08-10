
#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "function_body.hpp"
#include "simple_identifier.hpp"
#include "declaration.hpp"
#include "sequential_statement.hpp"
#include "interface_list.hpp"
#include "string.hpp"

namespace vhdl {
  namespace parser {
  
    FunctionBody* FunctionBody::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {};
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_FUNCTION);
      (scanner->optional<String>() || scanner->optional<SimpleIdentifier>());
      return this;
    }
    
  }
}

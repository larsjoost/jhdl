
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "function_declaration.hpp"
#include "basic_identifier.hpp"
#include "declaration.hpp"
#include "sequential_statement.hpp"
#include "interface_list.hpp"

namespace vhdl {
  namespace parser {
  
    FunctionDeclaration* FunctionDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_FUNCTION);
      name = scanner->expect<BasicIdentifier>();
      interface = scanner->expect<InterfaceList<scanner::Scanner::VHDL_VARIABLE>>();
      scanner->expect(scanner::Scanner::VHDL_RETURN);
      returnType = scanner->expect<BasicIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {};
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_FUNCTION);
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      if (!name->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match function name '" +
                       name->toString() + "'");
      }
      return this;
    }
    
  }
}

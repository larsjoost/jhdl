
#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "function_declaration.hpp"
#include "function_body.hpp"
#include "simple_identifier.hpp"
#include "declaration.hpp"
#include "sequential_statement.hpp"
#include "interface_list.hpp"

namespace vhdl {
  namespace parser {
  
    FunctionDeclaration* FunctionDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_FUNCTION);
      name = scanner->expect<SimpleIdentifier>();
      interface = scanner->expect<InterfaceList<scanner::Scanner::VHDL_VARIABLE>>();
      scanner->expect(scanner::Scanner::VHDL_RETURN);
      returnType = scanner->expect<SimpleIdentifier>();
      body = scanner->optional<FunctionBody>();
      return this;
    }
    
  }
}


#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../../ast/object_type.hpp"
#include "../scanner/scanner.hpp"
#include "procedure_declaration.hpp"
#include "procedure_body.hpp"
#include "simple_identifier.hpp"
#include "declaration.hpp"
#include "sequential_statement.hpp"
#include "interface_list.hpp"

namespace vhdl {
  namespace parser {
  
    ProcedureDeclaration* ProcedureDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_PROCEDURE);
      type = ast::ObjectType::PROCEDURE;
      name = scanner->expect<SimpleIdentifier, ProcedureDeclaration>();
      interface = scanner->optional<InterfaceList<scanner::Scanner::VHDL_VARIABLE>>();
      body = scanner->optional<ProcedureBody>();
      return this;
    }
    
  }
}

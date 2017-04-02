#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "attribute.hpp"
#include "simple_identifier.hpp"
#include "list.hpp"
#include "string.hpp"

namespace vhdl {
  namespace parser {
  
    Attribute* Attribute::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ATTRIBUTE);
      identifier = scanner->expect<SimpleIdentifier>();
      if (scanner->optional("[")) {
        arguments = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>>();
        scanner->expect("]");
      }
      scanner->expect(":");
      bool classFound =
        ((typeMark = scanner->optional<SimpleIdentifier>()) || 
         (scanner->optional(scanner::Scanner::VHDL_ENTITY)) || 
         (scanner->optional(scanner::Scanner::VHDL_PROCEDURE)) || 
         (scanner->optional(scanner::Scanner::VHDL_TYPE)) || 
         (scanner->optional(scanner::Scanner::VHDL_SIGNAL)) || 
         (scanner->optional(scanner::Scanner::VHDL_GROUP)) || 
         (scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE)) || 
         (scanner->optional(scanner::Scanner::VHDL_FUNCTION)) || 
         (scanner->optional(scanner::Scanner::VHDL_SUBTYPE)) || 
         (scanner->optional(scanner::Scanner::VHDL_VARIABLE)) || 
         (scanner->optional(scanner::Scanner::VHDL_FILE)) || 
         (scanner->optional(scanner::Scanner::VHDL_CONFIGURATION)) || 
         (scanner->optional(scanner::Scanner::VHDL_PACKAGE)) || 
         (scanner->optional(scanner::Scanner::VHDL_CONSTANT)) || 
         (scanner->optional(scanner::Scanner::VHDL_COMPONENT)));
      if (!classFound) {
        scanner->error("Did not find attribute class");
      }
      if (scanner->optional(scanner::Scanner::VHDL_IS)) {
        expression = scanner->expect<String>();
      }
      return this;
    }

  }
}


#include "../../ast/scanner.hpp"
#include "../../ast/object_type.hpp"
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
      if (scanner->optional(scanner::Scanner::VHDL_OF)) {
        if (!(string = scanner->optional<String>())) {
          item = scanner->expect<SimpleIdentifier>();
        }
        if (scanner->optional("[")) {
          arguments = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>>();
          scanner->expect("]");
        }
      }
      scanner->expect(":");
      if (typeMark = scanner->optional<SimpleIdentifier>()) {}
      else if (scanner->optional(scanner::Scanner::VHDL_ENTITY)) {objectType = ast::ENTITY;}
      else if  (scanner->optional(scanner::Scanner::VHDL_PROCEDURE)) {objectType = ast::PROCEDURE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_TYPE)) {objectType = ast::TYPE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_SIGNAL)) {objectType = ast::SIGNAL;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_GROUP)) {objectType = ast::GROUP;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE)) {objectType = ast::ARCHITECTURE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_FUNCTION)) {objectType = ast::FUNCTION;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_SUBTYPE)) {objectType = ast::SUBTYPE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_VARIABLE)) {objectType = ast::VARIABLE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_FILE)) {objectType = ast::FILE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_CONFIGURATION)) {objectType = ast::CONFIGURATION;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_PACKAGE)) {objectType = ast::PACKAGE;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_CONSTANT)) {objectType = ast::CONSTANT;} 
      else if  (scanner->optional(scanner::Scanner::VHDL_COMPONENT))  {objectType = ast::COMPONENT;}
      else {
        scanner->error("Did not find attribute class");
      }
      if (scanner->optional(scanner::Scanner::VHDL_IS)) {
        expression = scanner->expect<String>();
      }
      return this;
    }

  }
}


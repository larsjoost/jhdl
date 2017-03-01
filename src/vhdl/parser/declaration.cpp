#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_declaration.hpp"
#include "object_declaration.hpp"
#include "function_declaration.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Declaration* Declaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if ((type = scanner->optional<TypeDeclaration>()) || 
          (variable = scanner->optional<ObjectDeclaration<scanner::Scanner::VHDL_VARIABLE>>()) || 
          (signal = scanner->optional<ObjectDeclaration<scanner::Scanner::VHDL_SIGNAL>>()) || 
          (constant = scanner->optional<ObjectDeclaration<scanner::Scanner::VHDL_CONSTANT>>()) || 
          (function = scanner->optional<FunctionDeclaration>())) {
        scanner->expect(";");
      }
      return this;
    }

  }
}


#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_declaration.hpp"
#include "variable_declaration.hpp"
#include "signal_declaration.hpp"
#include "constant_declaration.hpp"
#include "function_declaration.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Declaration* Declaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if ((type = scanner->optional<TypeDeclaration>()) || 
          (variable = scanner->optional<VariableDeclaration>()) || 
          (signal = scanner->optional<SignalDeclaration>()) || 
          (constant = scanner->optional<ConstantDeclaration>()) || 
          (function = scanner->optional<FunctionDeclaration>())) {
        scanner->expect(";");
      }
      return this;
    }

  }
}


#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_definition.hpp"
#include "number_type.hpp"
#include "enumeration_type.hpp"

namespace vhdl {
  namespace parser {

    TypeDefinition* TypeDefinition::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ((numberType = scanner->optional<NumberType>()) ||
       (enumerationType = scanner->optional<EnumerationType>()));
      return this;
    }

  }
}


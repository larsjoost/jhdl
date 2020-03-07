#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "array_type.hpp"
#include "array_definition.hpp"
#include "subtype_indication.hpp"

namespace vhdl {
  namespace parser {
  
    ArrayType* ArrayType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ARRAY);
      scanner->expect("(");
      array_definition.add(scanner->expect<ArrayDefinition, ArrayType>());
      while (scanner->optional(",")) {
        array_definition.add(scanner->expect<ArrayDefinition, ArrayType>());
      }
      scanner->expect(")");
      scanner->accept(scanner::Scanner::VHDL_OF);
      subtype = scanner->optional<SubtypeIndication>();
      return this;
    }

  }
}

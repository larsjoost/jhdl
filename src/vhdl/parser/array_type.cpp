#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "array_type.hpp"
#include "basic_identifier_list.hpp"
#include "range_type.hpp"
#include "subtype_indication.hpp"

namespace vhdl {
  namespace parser {
  
    ArrayType* ArrayType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ARRAY);
      scanner->expect("(");
      range = scanner->expect<RangeType>();
      scanner->expect(")");
      scanner->accept(scanner::Scanner::VHDL_OF);
      type = scanner->optional<SubtypeIndication>();
      return this;
    }

  }
}

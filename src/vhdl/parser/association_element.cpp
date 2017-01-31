
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "expression.hpp"
#include "association_element.hpp"

namespace vhdl {
  namespace parser {
  
    AssociationElement* AssociationElement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      expression = scanner->expect<Expression>();
      return this;
    }

  }
}


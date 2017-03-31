#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "physical.hpp"
#include "number.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Physical* Physical::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      number = scanner->accept<Number>();
      unit = scanner->accept<SimpleIdentifier>();
      return this;
    }
    
  }
}

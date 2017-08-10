
#ifndef VHDL_PARSER_SCALARTYPE_H_
#define VHDL_PARSER_SCALARTYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/scalar_type.hpp"

namespace vhdl {
  namespace parser {

    class ScalarType :
      public ::ast::ScalarType  {

    public:

      ScalarType* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

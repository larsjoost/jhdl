
#ifndef VHDL_PARSER_RANGEDIRECTIONTYPE_H_
#define VHDL_PARSER_RANGEDIRECTIONTYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/range_direction_type.hpp"

namespace vhdl {
  namespace parser {

    class RangeDirectionType :
      public ::ast::RangeDirectionType  {

    public:

      RangeDirectionType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

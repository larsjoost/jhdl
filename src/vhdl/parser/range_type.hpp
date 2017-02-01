
#ifndef VHDL_PARSER_RANGETYPE_H_
#define VHDL_PARSER_RANGETYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/range_type.hpp"

namespace vhdl {
  namespace parser {

    class RangeType :
      public ::ast::RangeType  {

    public:

      RangeType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

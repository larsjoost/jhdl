
#ifndef VHDL_PARSER_RANGEATTRIBUTETYPE_H_
#define VHDL_PARSER_RANGEATTRIBUTETYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/range_attribute_type.hpp"

namespace vhdl {
  namespace parser {

    class RangeAttributeType :
      public ::ast::RangeAttributeType  {

    public:

      RangeAttributeType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

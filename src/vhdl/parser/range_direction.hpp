
#ifndef VHDL_PARSER_RANGEDIRECTION_H_
#define VHDL_PARSER_RANGEDIRECTION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/type_declaration.hpp"

namespace vhdl {
  namespace parser {

    class RangeDirection :
      public ::ast::RangeDirection  {

    public:

      RangeDirection* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

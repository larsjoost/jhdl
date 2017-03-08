
#ifndef VHDL_PARSER_SUBTYPEINDICATION_H_
#define VHDL_PARSER_SUBTYPEINDICATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/subtype_indication.hpp"

namespace vhdl {
  namespace parser {

    class SubtypeIndication :
      public ::ast::SubtypeIndication  {

    public:

      SubtypeIndication* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif


#ifndef VHDL_PARSER_PHYSICALELEMENT_H_
#define VHDL_PARSER_PHYSICALELEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/physical_element.hpp"

namespace vhdl {
  namespace parser {

    class PhysicalElement :
      public ::ast::PhysicalElement  {

    public:

      PhysicalElement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

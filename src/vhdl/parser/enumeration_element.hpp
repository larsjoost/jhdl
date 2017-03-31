
#ifndef VHDL_PARSER_ENUMERATIONELEMENT_H_
#define VHDL_PARSER_ENUMERATIONELEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/enumeration_element.hpp"

namespace vhdl {
  namespace parser {

    class EnumerationElement :
      public ::ast::EnumerationElement  {

    public:

      EnumerationElement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif


#ifndef VHDL_PARSER_ATTRIBUTE_H_
#define VHDL_PARSER_ATTRIBUTE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/attribute.hpp"

namespace vhdl {
  namespace parser {

    class Attribute :
      public ::ast::Attribute  {

    public:

      Attribute* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

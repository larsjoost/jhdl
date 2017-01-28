
#ifndef VHDL_PARSER_ENUMERATIONTYPE_H_
#define VHDL_PARSER_ENUMERATIONTYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/enumeration_type.hpp"

namespace vhdl {
  namespace parser {

    class EnumerationType :
      public ::ast::EnumerationType  {

    public:

      EnumerationType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

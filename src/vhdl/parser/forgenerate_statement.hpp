
#ifndef VHDL_PARSER_FORGENERATESTATEMENT_H_
#define VHDL_PARSER_FORGENERATESTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/forgenerate_statement.hpp"

namespace vhdl {
  namespace parser {

    class ForGenerateStatement :
      public ::ast::ForGenerateStatement  {

    public:

      ForGenerateStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

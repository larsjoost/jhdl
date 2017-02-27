
#ifndef VHDL_PARSER_RETURNSTATEMENT_H_
#define VHDL_PARSER_RETURNSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/return_statement.hpp"

namespace vhdl {
  namespace parser {

    class ReturnStatement :
      public ::ast::ReturnStatement  {

    public:

      ReturnStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

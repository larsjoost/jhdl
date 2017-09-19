
#ifndef VHDL_PARSER_LOOPSTATEMENT_H_
#define VHDL_PARSER_LOOPSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/loop_statement.hpp"

namespace vhdl {
  namespace parser {

    class LoopStatement :
      public ::ast::LoopStatement  {

    public:

      LoopStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

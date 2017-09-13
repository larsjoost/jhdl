
#ifndef VHDL_PARSER_WHILELOOPSTATEMENT_H_
#define VHDL_PARSER_WHILELOOPSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/whileloop_statement.hpp"

namespace vhdl {
  namespace parser {

    class WhileLoopStatement :
      public ::ast::WhileLoopStatement  {

    public:

      WhileLoopStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

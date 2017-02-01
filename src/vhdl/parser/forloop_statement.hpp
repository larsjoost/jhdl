
#ifndef VHDL_PARSER_FORLOOPSTATEMENT_H_
#define VHDL_PARSER_FORLOOPSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/forloop_statement.hpp"

namespace vhdl {
  namespace parser {

    class ForLoopStatement :
      public ::ast::ForLoopStatement  {

    public:

      ForLoopStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

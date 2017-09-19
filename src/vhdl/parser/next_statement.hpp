
#ifndef VHDL_PARSER_NEXTSTATEMENT_H_
#define VHDL_PARSER_NEXTSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/next_statement.hpp"

namespace vhdl {
  namespace parser {

    class NextStatement :
      public ::ast::NextStatement  {

    public:

      NextStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

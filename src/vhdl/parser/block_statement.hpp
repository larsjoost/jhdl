
#ifndef VHDL_PARSER_BLOCKSTATEMENT_H_
#define VHDL_PARSER_BLOCKSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/block_statement.hpp"

namespace vhdl {
  namespace parser {

    class BlockStatement :
      public ::ast::BlockStatement  {

    public:

      BlockStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

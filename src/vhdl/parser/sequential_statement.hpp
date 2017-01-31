
#ifndef VHDL_PARSER_SEQUENTIALSTATEMENT_H_
#define VHDL_PARSER_SEQUENTIALSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/sequential_statement.hpp"

namespace vhdl {
  namespace parser {

    class SequentialStatement :
      public ::ast::SequentialStatement {

    public:

      SequentialStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

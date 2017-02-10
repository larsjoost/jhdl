
#ifndef VHDL_PARSER_CONCURRENTSTATEMENT_H_
#define VHDL_PARSER_CONCURRENTSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/concurrent_statement.hpp"

namespace vhdl {
  namespace parser {

    class ConcurrentStatement :
      public ::ast::ConcurrentStatement {

    public:

      ConcurrentStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

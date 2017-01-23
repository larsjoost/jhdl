
#ifndef VHDL_PARSER_PROCESS_H_
#define VHDL_PARSER_PROCESS_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/method.hpp"

namespace vhdl {
  namespace parser {

    class Process :
      public ::ast::Method  {

    protected:

      void parseBody(::ast::Scanner<scanner::Scanner>* scanner);

    public:

      Process* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif


#ifndef VHDL_PARSER_ARCHITECTURE_H_
#define VHDL_PARSER_ARCHITECTURE_H_

#include "../scanner/scanner.hpp"
#include "../ast/architecture.hpp"

namespace vhdl {
  namespace parser {

    class Architecture :
      public ast::Architecture  {

    public:

      Architecture* parse(scanner::Scanner* s);

    };

  }

}
  

#endif

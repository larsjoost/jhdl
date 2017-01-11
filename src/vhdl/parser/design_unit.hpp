
#ifndef VHDL_PARSER_DESIGNUNIT_HPP_
#define VHDL_PARSER_DESIGNUNIT_HPP_

#include "../scanner/scanner.hpp"
#include "../ast/design_unit.hpp"

namespace vhdl {
  namespace parser {
    
    class DesignUnit :
      public ast::DesignUnit {
    
    public:

      void parse(scanner::Scanner* s);

    };
  }

}

#endif

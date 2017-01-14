
#ifndef VERILOG_PARSER_DESIGNUNIT_HPP_
#define VERILOG_PARSER_DESIGNUNIT_HPP_

#include "../scanner/scanner.hpp"
#include "../../ast/design_unit.hpp"

namespace verilog {
  namespace parser {
    
    class DesignUnit :
      public ::ast::DesignUnit {
    
    public:

      void parse(scanner::Scanner* s);

    };
  }

}

#endif

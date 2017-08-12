
#ifndef VHDL_PARSER_DESIGNUNIT_HPP_
#define VHDL_PARSER_DESIGNUNIT_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/design_unit.hpp"
#include "../../debug/debug.hpp"

namespace vhdl {
  namespace parser {
    
    class DesignUnit : public ::ast::DesignUnit {

      Debug<false> debug;
      
    public:

      DesignUnit() : debug("vhdl::parser::DesignUnit") {};
      
      void parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
  }

}

#endif

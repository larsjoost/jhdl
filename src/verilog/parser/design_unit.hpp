
#ifndef VERILOG_PARSER_DESIGNUNIT_HPP_
#define VERILOG_PARSER_DESIGNUNIT_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/design_unit.hpp"
#include "../../debug/debug.hpp"

namespace verilog {
  namespace parser {
    
    class DesignUnit : public ::ast::DesignUnit {
    
      Debug<false> m_debug;

    public:

      DesignUnit() : m_debug(this) {};

      void parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
  }

}

#endif

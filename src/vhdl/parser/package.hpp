
#ifndef VHDL_PARSER_PACKAGE_H_
#define VHDL_PARSER_PACKAGE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/package.hpp"
#include "../../debug/debug.hpp"

namespace vhdl {
  namespace parser {
  
    class Package : public ::ast::Package {

      Debug<false> m_debug;
      
    public:

      Package() : m_debug(this) {};
      
      Package* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif


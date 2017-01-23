#ifndef VERILOG_PARSER_DESIGNFILE_HPP_
#define VERILOG_PARSER_DESIGNFILE_HPP_

#include "../../ast/scanner.hpp"
#include "../../ast/design_file.hpp"
#include "../scanner/scanner.hpp"

namespace verilog {
  namespace parser {
  
    class DesignFile :
      public ::ast::DesignFile {

    public:
      
      DesignFile* parse(::ast::Scanner<scanner::Scanner>* s);
    
    };

  }
}

#endif

#ifndef VHDL_PARSER_DESIGNFILE_HPP_
#define VHDL_PARSER_DESIGNFILE_HPP_

#include "../scanner/scanner.hpp"
#include "../ast/design_file.hpp"

namespace vhdl {
  namespace parser {
  
    class DesignFile :
      public ast::DesignFile {

    public:

      void parse(scanner::Scanner* s);
    
    };

  }
}

#endif

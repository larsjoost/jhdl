#ifndef VHDL_PARSER_FILE_DECLARATION_HPP_
#define VHDL_PARSER_FILE_DECLARATION_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/file_declaration.hpp"

namespace vhdl {
  namespace parser {
    
    class FileDeclaration :
      public ::ast::FileDeclaration {
    
    public:

      FileDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif

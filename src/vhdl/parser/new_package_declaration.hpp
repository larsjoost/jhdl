
#ifndef VHDL_PARSER_NEW_PACKAGE_DECLARATION_HPP__
#define VHDL_PARSER_NEW_PACKAGE_DECLARATION_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/new_package_declaration.hpp"
#include "../../debug/debug.hpp"

namespace vhdl {
  namespace parser {
  
    class NewPackageDeclaration : public ::ast::NewPackageDeclaration {

      Debug<false> debug;
      
    public:

      NewPackageDeclaration() : debug("vhdl::parser::NewPackageDeclaration") {};
      
      NewPackageDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

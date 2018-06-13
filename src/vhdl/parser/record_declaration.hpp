
#ifndef VHDL_PARSER_RECORDDECLARATION_H_
#define VHDL_PARSER_RECORDDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/record_declaration.hpp"

namespace vhdl {
  namespace parser {

    class RecordDeclaration :
      public ::ast::RecordDeclaration  {

    public:

      RecordDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif

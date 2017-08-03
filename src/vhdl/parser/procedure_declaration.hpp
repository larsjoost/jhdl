
#ifndef VHDL_PARSER_PROCEDUREDECLARATION_H_
#define VHDL_PARSER_PROCEDUREDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/function_declaration.hpp"

namespace vhdl {
  namespace parser {

    class ProcedureDeclaration :
      public ::ast::FunctionDeclaration  {

    public:

      ProcedureDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

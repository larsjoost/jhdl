
#ifndef VHDL_PARSER_ASSOCIATION_LIST_H_
#define VHDL_PARSER_ASSOCIATION_LIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/association_list.hpp"

namespace vhdl {
  namespace parser {

    class AssociationList :
      public ::ast::AssociationList  {

    public:

      AssociationList* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif

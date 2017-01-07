/*
 * Scanner.h
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_VHDL_SCANNER_H_
#define SRC_VHDL_SCANNER_H_

#include <array>

#include "../ast/Scanner.h"
#include "../ast/identifier.h"
#include "defines.h"

namespace vhdl {

  class BasicIdentifier : public ast::Identifier {
  public:
    using ast::Identifier::equals;
    using ast::Identifier::print;
  };

  class Scanner : public ast::Scanner {

  
  public:
    
    int verbose;

    Scanner(int verbose = 0);
    
    void accept(defines::VhdlKeyword keyword);
    int accept(BasicIdentifier& i);
    void expect(defines::VhdlKeyword keyword);
    void expect(BasicIdentifier& i);
    int optional(defines::VhdlKeyword keyword);
    
    void skipWhiteSpaceAndComments();
    
    using ast::Scanner::getPosition;
    using ast::Scanner::accept;
    using ast::Scanner::expect;
    using ast::Scanner::optional;
    using ast::Scanner::getText;
    using ast::Scanner::lookAhead;
    using ast::Scanner::incrementPosition;
    using ast::Scanner::getNumberOfErrors;
    using ast::Scanner::skipOneOrMoreWhiteSpaces;
  };
  
}



#endif /* SRC_VHDL_SCANNER_H_ */


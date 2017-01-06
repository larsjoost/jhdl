/*
 * Scanner.h
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_VHDL_SCANNER_H_
#define SRC_VHDL_SCANNER_H_

#include "../ast/Scanner.h"

namespace vhdl {

  class Identifier : public ast::Identifier {

  public:
    using ast::Identifier::equals;
  };

  class Scanner : public ast::Scanner {

  public:

    enum VhdlKeyword {
      VHDL_ARCHITECTURE,
      VHDL_IS,
      VHDL_BEGIN,
      VHDL_END,
      VHDL_ENTITY,
      NUMBER_OF_VHDL_KEYWORDS
    };
    
    enum VhdlStandard {
      VHDL_1987,
      VHDL_1993,
      VHDL_2002,
      VHDL_2008,
      NUMBER_OF_VHDL_STANDARDS
    };
    
  private:
    
    struct VhdlKeywordInfo {
      VhdlKeyword keyword;
      char* text;
      VhdlStandard standard;
    };
    
    const VhdlKeywordInfo VHDL_KEYWORD_INFO[NUMBER_OF_VHDL_KEYWORDS] {
      {VHDL_ARCHITECTURE, "architecture", VHDL_1987},
      {VHDL_IS, "is", VHDL_1987},
      {VHDL_BEGIN, "begin", VHDL_1987},
      {VHDL_END, "end", VHDL_1987},
      {VHDL_ENTITY, "entity", VHDL_1987}
    };
    
    char** VhdlKeywordLookup;
    
  public:
    
    Scanner();
    
    int accept(VhdlKeyword keyword);
    int accept(Identifier& i);
    int expect(VhdlKeyword keyword);
    int expect(Identifier& i);
    int optional(VhdlKeyword keyword);
    
    void skipWhiteSpaceAndComments();
    
    using ast::Scanner::getPosition;
    using ast::Scanner::accept;
    using ast::Scanner::expect;
    using ast::Scanner::optional;
    using ast::Scanner::getText;
    using ast::Scanner::lookAhead;
    using ast::Scanner::incrementPosition;
  };
  
}



#endif /* SRC_VHDL_SCANNER_H_ */


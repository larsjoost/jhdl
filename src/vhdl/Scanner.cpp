/*
 * Scanner.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Scanner.h"
#include "../ast/Text.h"
#include "../ast/Scanner.h"

namespace vhdl {

  Scanner::Scanner() {
    VhdlKeywordLookup = (char **)malloc(sizeof(char*) * NUMBER_OF_VHDL_KEYWORDS);
    for (int i=0; i<NUMBER_OF_VHDL_KEYWORDS; i++) {
      int x = VHDL_KEYWORD_INFO[i].keyword;
      VhdlKeywordLookup[x] = VHDL_KEYWORD_INFO[i].text;
    }
  }

  int Scanner::accept(Scanner::VhdlKeyword keyword) {
    return accept(VhdlKeywordLookup[keyword]);
  }

  int Scanner::expect(Scanner::VhdlKeyword keyword) {
    return expect(VhdlKeywordLookup[keyword]);
  }

  int Scanner::optional(Scanner::VhdlKeyword keyword) {
    return optional(VhdlKeywordLookup[keyword]);
  }

  int Scanner::accept(Identifier& id) {
    getText(id.text);
    int i = 0;
    char a = lookAhead(0);
    // IMPROVE !
    while ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_') {
      i++;
      incrementPosition();
      a = lookAhead(0);
    }
    id.text.setSize(i);
    return i;
  }

  int Scanner::expect(Identifier& id) {
    int len = accept(id);
    if (!len) {
      throw ast::UnexpectedToken(); 
    }
    return len;
  }
  
  void Scanner::skipWhiteSpaceAndComments() {

	try {
		while (skipWhiteSpace()) {
			if (accept("--")) {
				skipUntilEndOfLine();
			}
		}
	} catch (ast::TextEof e) {

	}
}

}


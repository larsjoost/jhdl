/*
 * Scanner.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Scanner.h"
#include "../ast/Text.h"

namespace vhdl {

Scanner::Scanner() {
	VhdlKeywordLookup = new char*[NUMBER_OF_VHDL_KEYWORDS];
	for (int i=0; i<NUMBER_OF_VHDL_KEYWORDS; i++) {
		VhdlKeywordLookup[VHDL_KEYWORD_INFO[i].keyword] = VHDL_KEYWORD_INFO[i].text;
	}
}

int Scanner::accept(Scanner::VhdlKeyword keyword) {
	return accept(VhdlKeywordLookup[keyword]);
}

int Scanner::accept(Identifier& id) {
	getText(id.text);
	int i = 0;
	char a = text.lookAhead(0);
	// IMPROVE !
	while ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_') {
		i++;
		text.incrementPosition();
		a = text.lookAhead(0);
	}
	id.text.setSize(i);
	return i;
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


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

};

class Scanner : public ast::Scanner {

public:

	enum VhdlKeyword {
		VHDL_ARCHITECTURE,
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
		const VhdlKeyword keyword;
		const char* text;
		const VhdlStandard standard;
	};

	const VhdlKeywordInfo VHDL_KEYWORD_INFO[NUMBER_OF_VHDL_KEYWORDS] {
		{VHDL_ARCHITECTURE, "architecture", VHDL_1987},
		{VHDL_ENTITY, "entity", VHDL_1987}
	};

	const char* VhdlKeywordLookup[];

public:

	Scanner();

	int accept(VhdlKeyword keyword);
	int accept(Identifier& i);

	void skipWhiteSpaceAndComments();

	using ast::Scanner::accept;
	using ast::Scanner::getText;

};

}



#endif /* SRC_VHDL_SCANNER_H_ */


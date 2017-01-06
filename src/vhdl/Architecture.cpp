/*
 * Architecture.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Architecture.h"
#include "Scanner.h"

namespace vhdl {

ArchitectureParser::ArchitectureParser(Scanner& s) {
	mScanner = s;
}

ArchitectureParser* ArchitectureParser::optional() {
	ArchitectureParser* result = NULL;
	try {
		mScanner.skipWhiteSpaceAndComments();
		mScanner.expect(Scanner::VHDL_ARCHITECTURE);
		result = new ArchitectureParser(mScanner);
		mScanner.expect(result->name));
		mScanner.expect(Scanner::VHDL_IS);
		mScanner.expect(Scanner::VHDL_BEGIN);
		mScanner.expect(Scanner::VHDL_END);
		mScanner.optional(Scanner::VHDL_ARCHITECTURE);
		Identifier name;
		mScanner.expect(name);
		if (!name.equals(result->name)) {
			mScanner.error("Architecture terminator name did not match architecture name");
			throw UnexpectedToken();
		}
		mScanner.expect(";");
		return result;
	} catch (UnexpectedToken e) {
		mScanner.error("Did not expect this token");
		if (result) {delete result;}
	}
	return NULL;
}

}


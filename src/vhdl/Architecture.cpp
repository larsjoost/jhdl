/*
 * Architecture.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Architecture.h"
#include "Scanner.h"

namespace vhdl {

Architecture::Architecture(Scanner& s) {
	mScanner = s;
}

Architecture* Architecture::parse() {
	mScanner.skipWhiteSpaceAndComments();
	Identifier i;
	if (mScanner.accept(i)) {
		name.set(i.text);
	}

	return this;
}

}


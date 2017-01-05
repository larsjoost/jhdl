/*
 * VhdlDesignUnit.cpp
 *
 *  Created on: 4 Jan 2017
 *      Author: lars_
 */

#include "DesignUnit.h"
#include "Architecture.h"

namespace vhdl {

DesignUnit::DesignUnit(Scanner& v) {
	mScanner = v;
}

void DesignUnit::addArchitecture(Architecture* a) {
	const char* c = a->name();
	architectures[a->name()] = a;
}

DesignUnit& DesignUnit::parse() {
	try {
		mScanner.skipWhiteSpaceAndComments();
		if (mScanner.accept(Scanner::VHDL_ARCHITECTURE)) {
			addArchitecture((new Architecture(mScanner))->parse());
		}
	} catch (ast::TextEof e) {}
	return this;
}

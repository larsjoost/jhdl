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
	ArchitectureParser architecture;
	EntityParser entity;
	try {
		mScanner.skipWhiteSpaceAndComments();
		int position = mScanner.getPosition();
		add(architecture.optional());
		add(entity.optional());
		if (mScanner.getPosition() == position) {
			mScanner.critical("Did not find anything");
		}
	} catch (ast::TextEof e) {}
	return this;
}

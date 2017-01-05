/*
 * VhdlDesignUnit.hpp
 *
 *  Created on: 4 Jan 2017
 *      Author: lars_
 */

#ifndef VHDLDESIGNUNIT_HPP_
#define VHDLDESIGNUNIT_HPP_

#include <map>
#include "Scanner.h"

namespace vhdl {

class DesignUnit {

	Scanner mScanner;

	std::map<std::string, Architecture&> architectures;

	void addArchitecture(Architecture* a);

public:

	DesignUnit(Scanner& v);


	DesignUnit& parse();
};

}

#endif /* VHDLDESIGNUNIT_HPP_ */

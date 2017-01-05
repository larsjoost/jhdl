/*
 * Architecture.h
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_VHDL_ARCHITECTURE_H_
#define SRC_VHDL_ARCHITECTURE_H_

#include "Scanner.h"
#include "../ast/Text.h"

namespace vhdl {

class ArchitectureNameNull {};

class Architecture {

	 Scanner mScanner;

	 ast::Text name;

public:

	Architecture(Scanner& s);

	Architecture* parse();

	char* getName();

};

}



#endif /* SRC_VHDL_ARCHITECTURE_H_ */

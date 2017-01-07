/*
 * Architecture.h
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_VHDL_ARCHITECTURE_H_
#define SRC_VHDL_ARCHITECTURE_H_

#include "Scanner.h"

namespace vhdl {

  class ArchitectureNameNull {};

  class ArchitectureParser {

    Scanner* mScanner;

  public:

    BasicIdentifier name;

    ArchitectureParser(Scanner* s);

    ArchitectureParser* optional();

  };

}

#endif /* SRC_VHDL_ARCHITECTURE_H_ */

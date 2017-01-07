/*
 * Entity.h
 *
 *  Created on: 6 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_VHDL_ENTITY_H_
#define SRC_VHDL_ENTITY_H_

#include "Scanner.h"

namespace vhdl {

  class EntityParser {
	
    Scanner* mScanner;
    
  public:

    BasicIdentifier name;

    EntityParser(Scanner* s);

    EntityParser* optional();

  };

}

#endif /* SRC_VHDL_ENTITY_H_ */


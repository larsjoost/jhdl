/*
 * VhdlDesignUnit.hpp
 *
 *  Created on: 4 Jan 2017
 *      Author: lars_
 */

#ifndef VHDLDESIGNUNIT_HPP_
#define VHDLDESIGNUNIT_HPP_

#include <list>
#include "Scanner.h"
#include "Architecture.h"
#include "Entity.h"

namespace vhdl {
  
  class DesignUnit {
    
    Scanner* mScanner;

    std::list<ArchitectureParser*> architectures;
    std::list<EntityParser*> entities;

    void add(ArchitectureParser* a);
    void add(EntityParser* e);

    ArchitectureParser* architecture;
    EntityParser* entity;

  public:

    DesignUnit(Scanner* v);

    DesignUnit* parse();
  };

}

#endif /* VHDLDESIGNUNIT_HPP_ */

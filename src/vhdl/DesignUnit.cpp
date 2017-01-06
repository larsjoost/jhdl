/*
 * VhdlDesignUnit.cpp
 *
 *  Created on: 4 Jan 2017
 *      Author: lars_
 */

#include "DesignUnit.h"
#include "Architecture.h"
#include "Entity.h"

namespace vhdl {

  DesignUnit::DesignUnit(Scanner* v) {
    mScanner = v;
    architecture = new ArchitectureParser(v);
    entity = new EntityParser(v);
  }

  void DesignUnit::add(ArchitectureParser* a) {
    architectures.push_back(a);
  }

  void DesignUnit::add(EntityParser* e) {
    entities.push_back(e);
  }

  DesignUnit* DesignUnit::parse() {
    try {
      mScanner->skipWhiteSpaceAndComments();
      int position = mScanner->getPosition();
      add(architecture->optional());
      add(entity->optional());
      if (mScanner->getPosition() == position) {
        mScanner->critical("Did not find anything");
      }
    } catch (ast::TextEof e) {}
    return this;
  }

}

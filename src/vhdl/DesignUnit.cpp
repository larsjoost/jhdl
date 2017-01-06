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
    if (a != NULL) {
      architectures.push_back(a);
    }
  }

  void DesignUnit::add(EntityParser* e) {
    if (e != NULL) {
      entities.push_back(e);
    }
  }

  DesignUnit* DesignUnit::parse() {
    try {
      while (1) {
        mScanner->skipWhiteSpaceAndComments();
        int position = mScanner->getPosition();
        add(entity->optional());
        add(architecture->optional());
        if (mScanner->getPosition() == position) {
          mScanner->critical("Did not find anything");
        }
      }
    } catch (ast::TextEof e) {
      printf("Reached end of file...\n");
    }
    return this;
  }

}

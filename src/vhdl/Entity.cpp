/*
 * Entity.cpp
 *
 *  Created on: 6 Jan 2017
 *      Author: lars_
 */

#include "Entity.h"
#include "Scanner.h"
#include "../ast/Scanner.h"

namespace vhdl {

  EntityParser::EntityParser(Scanner* s) {
    mScanner = s;
  }

  EntityParser* EntityParser::optional() {
    EntityParser* result = NULL;
    try {
      mScanner->skipWhiteSpaceAndComments();
      mScanner->accept(Scanner::VHDL_ENTITY);
      result = new EntityParser(mScanner);
      mScanner->expect(result->name);
      mScanner->expect(Scanner::VHDL_IS);
      mScanner->expect(Scanner::VHDL_END);
      mScanner->optional(Scanner::VHDL_ENTITY);
      Identifier name = Identifier();
      mScanner->expect(name);
      if (!name.equals(result->name)) {
        mScanner->error("Entity terminator name did not match entity name");
        throw ast::UnexpectedToken("entity end name");
      }
      mScanner->expect(";");
      return result;
    } catch (ast::UnexpectedToken e) {
      mScanner->error("Expected %s", e.text);
      if (result) {delete result;}
    } catch (ast::TokenNotAccepted e) {
      if (mScanner->verbose) {
        printf("Not an entity\n");
      }
    }

    return NULL;
  }
  
}



/*
 * Entity.cpp
 *
 *  Created on: 6 Jan 2017
 *      Author: lars_
 */

#include "defines.h"
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
      mScanner->accept(defines::VHDL_ENTITY);
      mScanner->skipOneOrMoreWhiteSpaces();
      result = new EntityParser(mScanner);
      mScanner->expect(result->name);
      mScanner->skipOneOrMoreWhiteSpaces();
      mScanner->expect(defines::VHDL_IS);
      mScanner->skipOneOrMoreWhiteSpaces();
      mScanner->expect(defines::VHDL_END);
      mScanner->skipOneOrMoreWhiteSpaces();
      mScanner->optional(defines::VHDL_ENTITY);
      mScanner->skipOneOrMoreWhiteSpaces();
      static BasicIdentifier name = BasicIdentifier();
      mScanner->expect(name);
      if (!name.equals(result->name)) {
        mScanner->error("Entity terminator name did not match entity name");
        throw ast::UnexpectedToken("entity end name");
      }
      mScanner->skipWhiteSpace();
      mScanner->expect(";");
      return result;
    } catch (ast::UnexpectedToken e) {
      mScanner->error("Expected %s", e.text);
      if (result) {delete result;}
    } catch (ast::TokenNotAccepted e) {
      if (mScanner->verbose) {
        printf("Not an entity\n");
      }
    } catch (ast::NoWhiteSpace e) {
      mScanner->error("Expected white space");
      if (result) {delete result;}
    }

    return NULL;
  }
  
}



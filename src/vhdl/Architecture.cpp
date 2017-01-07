/*
 * Architecture.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Architecture.h"
#include "Scanner.h"
#include "defines.h"
#include "../ast/Scanner.h"

namespace vhdl {

  ArchitectureParser::ArchitectureParser(Scanner* s) {
    mScanner = s;
  }

  ArchitectureParser* ArchitectureParser::optional() {
    ArchitectureParser* result = NULL;
    try {
      mScanner->skipWhiteSpaceAndComments();
      mScanner->accept(defines::VHDL_ARCHITECTURE);
      mScanner->skipOneOrMoreWhiteSpaces();
      result = new ArchitectureParser(mScanner);
      mScanner->expect(result->name);
      mScanner->skipOneOrMoreWhiteSpaces();
      mScanner->expect(defines::VHDL_IS);
      mScanner->skipOneOrMoreWhiteSpaces();
      mScanner->expect(defines::VHDL_BEGIN);
      mScanner->skipOneOrMoreWhiteSpaces();
      mScanner->expect(defines::VHDL_END);
      mScanner->skipOneOrMoreWhiteSpaces();
      if (mScanner->optional(defines::VHDL_ARCHITECTURE)) {
        mScanner->skipOneOrMoreWhiteSpaces();
      };
      BasicIdentifier name = BasicIdentifier();
      mScanner->expect(name);
      if (!name.equals(result->name)) {
        mScanner->error("Architecture terminator name did not match architecture name");
        throw ast::UnexpectedToken("architecture end name");
      }
      mScanner->skipWhiteSpace();
      mScanner->expect(";");
      return result;
    } catch (ast::UnexpectedToken e) {
      mScanner->error("Did not expect this token");
      if (result) {delete result;}
    } catch (ast::TokenNotAccepted e) {
      if (mScanner->verbose) {
        printf("Not an entity\n");
      }
    }
    return NULL;
  }
  
}


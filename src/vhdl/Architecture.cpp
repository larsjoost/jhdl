/*
 * Architecture.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Architecture.h"
#include "Scanner.h"
#include "../ast/Scanner.h"

namespace vhdl {

  ArchitectureParser::ArchitectureParser(Scanner* s) {
    mScanner = s;
  }

  ArchitectureParser* ArchitectureParser::optional() {
    ArchitectureParser* result = NULL;
    try {
      mScanner->skipWhiteSpaceAndComments();
      mScanner->accept(Scanner::VHDL_ARCHITECTURE);
      result = new ArchitectureParser(mScanner);
      mScanner->expect(result->name);
      mScanner->expect(Scanner::VHDL_IS);
      mScanner->expect(Scanner::VHDL_BEGIN);
      mScanner->expect(Scanner::VHDL_END);
      mScanner->optional(Scanner::VHDL_ARCHITECTURE);
      Identifier name = Identifier();
      mScanner->expect(name);
      if (!name.equals(result->name)) {
        mScanner->error("Architecture terminator name did not match architecture name");
        throw ast::UnexpectedToken("architecture end name");
      }
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


/*
 * Scanner.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include <cassert>

#include "Scanner.h"
#include "../ast/Text.h"
#include "../ast/Scanner.h"

#include "defines.h"

namespace vhdl {

  Scanner::Scanner(int v) : ast::Scanner(0, v) {
    verbose = v;
  }

  struct VhdlKeywordInfo {
    defines::VhdlKeyword keyword;
    char* text;
    defines::VhdlStandard standard;
  };
    
  static const VhdlKeywordInfo* getVhdlKeywordInfo(int i) {
    static const VhdlKeywordInfo VHDL_KEYWORD_INFO[defines::NUMBER_OF_VHDL_KEYWORDS] {
      {defines::VHDL_ARCHITECTURE, "architecture", defines::VHDL_1987},
      {defines::VHDL_IS, "is", defines::VHDL_1987},
      {defines::VHDL_BEGIN, "begin", defines::VHDL_1987},
      {defines::VHDL_END, "end", defines::VHDL_1987},
      {defines::VHDL_ENTITY, "entity", defines::VHDL_1987}
    };
    assert(i >= defines::NUMBER_OF_VHDL_KEYWORDS);
    return &VHDL_KEYWORD_INFO[i];
  }
    
  static const char* keywordLookup(defines::VhdlKeyword keyword) {
    static const struct VhdlKeywordLookup
    {
      std::array<char *, defines::NUMBER_OF_VHDL_KEYWORDS> LUT;

      VhdlKeywordLookup() {
        for (int i=0; i<defines::NUMBER_OF_VHDL_KEYWORDS; i++) {
          const VhdlKeywordInfo* k = getVhdlKeywordInfo(i);
          int x = k->keyword;
          LUT[x] = k->text;
        }
      }
    } KEYWORD;
    return KEYWORD.LUT[keyword];
  }

  void Scanner::accept(defines::VhdlKeyword keyword) {
    accept(keywordLookup(keyword));
  }

  void Scanner::expect(defines::VhdlKeyword keyword) {
    expect(keywordLookup(keyword));
  }

  int Scanner::optional(defines::VhdlKeyword keyword) {
    return optional(keywordLookup(keyword));
  }

  int Scanner::accept(BasicIdentifier& id) {
    static const int LUT_SIZE = sizeof(char) * 256;
    static const struct VhdlBasicIdentifierLookup
    {
      std::array<int, LUT_SIZE> VALID_FIRST_CHAR;
      std::array<int, LUT_SIZE> VALID_CHAR;

      void assignRange(std::array<int, LUT_SIZE>& lut, char start, char stop, int value) {
        for (int i=start; i <= stop; i++) {
          lut[i] = value;
        }
      }
      
      void assignRange(char start, char stop, int value) {
        for (int i=start; i <= stop; i++) {
          VALID_FIRST_CHAR[i] = VALID_CHAR[i] = value;
        }
      }

      VhdlBasicIdentifierLookup() {
        assignRange(0, LUT_SIZE-1, false);
        assignRange('a', 'z', true);
        assignRange('A', 'Z', true);
        assignRange(VALID_CHAR, '0', '9', true);
        assignRange(VALID_CHAR, '_', '_', true);
      }
    } BASIC_IDENTIFIER;
    getText(id.text);
    int i = 0;
    char a = lookAhead(0);
    if (!BASIC_IDENTIFIER.VALID_FIRST_CHAR[a]) {
      return 0;
    }
    while (BASIC_IDENTIFIER.VALID_CHAR[a]) {
      i++;
      incrementPosition();
      a = lookAhead(0);
    }
    id.text.setSize(i);
    return i;
  }

  void Scanner::expect(BasicIdentifier& id) {
    int len = accept(id);
    if (!len) {
      throw ast::UnexpectedToken("basic identifier"); 
    }
  }
  
  void Scanner::skipWhiteSpaceAndComments() {
    try {
      while (skipWhiteSpace()) {
        if (accept("--")) {
          skipUntilEndOfLine();
        }
      }
    } catch (ast::TextEof e) {
      
    }
  }

}


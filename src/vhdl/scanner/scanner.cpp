
#include <cassert>

#include "../../ast/text.hpp"
#include "../../ast/scanner.hpp"

#include "scanner.hpp"
#include "defines.hpp"

namespace vhdl {
  namespace scanner {
    
  Scanner::Scanner(int v) : ast::Scanner(0, v) {
    verbose = v;
  }

  struct VhdlKeywordInfo {
    VhdlKeyword keyword;
    char* text;
    VhdlStandard standard;
  };
    
  static const VhdlKeywordInfo* getVhdlKeywordInfo(int i) {
    static const VhdlKeywordInfo VHDL_KEYWORD_INFO[NUMBER_OF_VHDL_KEYWORDS] {
      {VHDL_LIBRARY, "library", VHDL_1987},
      {VHDL_USE, "use", VHDL_1987},
      {VHDL_ARCHITECTURE, "architecture", VHDL_1987},
      {VHDL_OF, "of", VHDL_1987},
      {VHDL_IS, "is", VHDL_1987},
      {VHDL_BEGIN, "begin", VHDL_1987},
      {VHDL_END, "end", VHDL_1987},
      {VHDL_ENTITY, "entity", VHDL_1987}
    };
    assert(i < NUMBER_OF_VHDL_KEYWORDS);
    return &VHDL_KEYWORD_INFO[i];
  }
    
  static const char* keywordLookup(VhdlKeyword keyword) {
    static const struct VhdlKeywordLookup
    {
      std::array<char *, NUMBER_OF_VHDL_KEYWORDS> LUT;

      VhdlKeywordLookup() {
        for (int i=0; i<NUMBER_OF_VHDL_KEYWORDS; i++) {
          const VhdlKeywordInfo* k = getVhdlKeywordInfo(i);
          int x = k->keyword;
          LUT[x] = k->text;
        }
      }
    } KEYWORD;
    return KEYWORD.LUT[keyword];
  }

  void Scanner::accept(VhdlKeyword keyword) {
    accept(keywordLookup(keyword));
  }

  void Scanner::expect(VhdlKeyword keyword) {
    const char* a = keywordLookup(keyword);
    try {
      expect(a);
    } catch (ast::UnexpectedToken e) {
      error("Expected keyword %s", a);
      throw e;
    }
  }

  int Scanner::optional(VhdlKeyword keyword) {
    return optional(keywordLookup(keyword));
  }

  int Scanner::accept(BasicIdentifier& id) {
    static const int LUT_SIZE = sizeof(char) * 256;
    static const struct VhdlBasicIdentifierLookup
    {
      std::array<char, LUT_SIZE> VALID_FIRST_CHAR;
      std::array<char, LUT_SIZE> VALID_CHAR;

      void assignRange(std::array<char, LUT_SIZE>& lut, char start, char stop, char value) {
        for (int i=start; i <= stop; i++) {
          lut[i] = value;
        }
      }
      
      void assignRange(char start, char stop, char value) {
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
    char a = lookAhead(0);
    if (!BASIC_IDENTIFIER.VALID_FIRST_CHAR[a]) {
      return 0;
    }
    int i = 0;
    do {
      a = lookAhead(++i);
    } while (BASIC_IDENTIFIER.VALID_CHAR[a]);
    eat(id.text, i);
    return i;
  }

  void Scanner::expect(BasicIdentifier& id) {
    int len = accept(id);
    if (!len) {
      throw ast::UnexpectedToken("basic identifier"); 
    }
  }

  void Scanner::skipWhiteSpaceAndComments() {
    while (skipWhiteSpace()) {
      try {
        accept("--");
        skipUntilEndOfLine();
      } catch (ast::TokenNotAccepted e) {
      }
    }
  }
    
  }
}


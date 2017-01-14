
#include <cassert>

#include "../../ast/text.hpp"
#include "../../ast/scanner.hpp"

#include "scanner.hpp"
#include "defines.hpp"

namespace verilog {
  namespace scanner {
    
  Scanner::Scanner(int v) : ast::Scanner(0, v) {
    verbose = v;
  }

  struct VerilogKeywordInfo {
    VerilogKeyword keyword;
    char* text;
    VerilogStandard standard;
  };
    
  static const VerilogKeywordInfo* getVerilogKeywordInfo(int i) {
    static const VerilogKeywordInfo VERILOG_KEYWORD_INFO[NUMBER_OF_VERILOG_KEYWORDS] {
      {VERILOG_MODULE, (char *)"module", VERILOG_1995},
      {VERILOG_ENDMODULE, (char *)"endmodule", VERILOG_1995}
    };
    assert(i < NUMBER_OF_VERILOG_KEYWORDS);
    return &VERILOG_KEYWORD_INFO[i];
  }
    
  static const char* keywordLookup(VerilogKeyword keyword) {
    static const struct VerilogKeywordLookup
    {
      std::array<char *, NUMBER_OF_VERILOG_KEYWORDS> LUT;

      VerilogKeywordLookup() {
        for (int i=0; i<NUMBER_OF_VERILOG_KEYWORDS; i++) {
          const VerilogKeywordInfo* k = getVerilogKeywordInfo(i);
          int x = k->keyword;
          LUT[x] = k->text;
        }
      }
    } KEYWORD;
    return KEYWORD.LUT[keyword];
  }

  void Scanner::accept(VerilogKeyword keyword) {
    accept(keywordLookup(keyword));
  }

  void Scanner::expect(VerilogKeyword keyword) {
    const char* a = keywordLookup(keyword);
    expect(a);
  }

  int Scanner::optional(VerilogKeyword keyword) {
    return optional(keywordLookup(keyword));
  }

  int Scanner::accept(BasicIdentifier& id) {
    static const int LUT_SIZE = sizeof(char) * 256;
    static const struct VerilogBasicIdentifierLookup
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

      VerilogBasicIdentifierLookup() {
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
        accept("//");
        skipUntilEndOfLine();
      } catch (ast::TokenNotAccepted e) {
      }
    }
  }
    
  }
}


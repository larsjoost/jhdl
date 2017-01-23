
#ifndef VERILOG_SCANNER_SCANNER_H_
#define VERILOG_SCANNER_SCANNER_H_

#include <array>

#include "../../ast/token_scanner.hpp"

namespace verilog {
  namespace scanner {
    
    class Scanner {
      
    public:
      
      static const int NUMBER_OF_SPECIAL_CHARACTERS = 5;
      
      static char* getSpecialCharacters() {
        static char a[NUMBER_OF_SPECIAL_CHARACTERS] =  {'(', ')', '=', '>', '<'};
        return a;
      }

      static const bool CASE_SENSITIVE = true;

      enum Keyword {
        VERILOG_MODULE,
        VERILOG_ENDMODULE,
        NUMBER_OF_KEYWORDS
      };
    
      enum Standard {
        VERILOG_1995,
        VERILOG_2001,
        NUMBER_OF_STANDARDS
      };
      
      struct KeywordInfo {
        Keyword keyword;
        char* text;
        Standard standard;
      };

      static KeywordInfo* getKeywordInfo() { 
        static KeywordInfo a[NUMBER_OF_KEYWORDS] = {
          {VERILOG_MODULE, (char *)"module", VERILOG_1995},
          {VERILOG_ENDMODULE, (char *)"endmodule", VERILOG_1995}
        };
        return a;
      }

      int skipWhiteSpaceAndComments(::ast::TokenScanner<Scanner>* scanner);
      
    };

  }
}

#endif



#ifndef VHDL_SCANNER_SCANNER_H_
#define VHDL_SCANNER_SCANNER_H_

#include <iostream>
#include <vector>
#include <typeinfo>

#include "../../ast/token_scanner.hpp"

namespace vhdl {
  namespace scanner {
    
    class Scanner {
  
    public:

      static const int NUMBER_OF_SPECIAL_CHARACTERS = 8;

      static char* getSpecialCharacters() {
        static char a[NUMBER_OF_SPECIAL_CHARACTERS] =
          {'(', ')', '\'', '=', '>', '<', ';', '.'};
        return a;
      }
        
      static const bool CASE_SENSITIVE = false;
      
      enum Keyword {
        VHDL_LIBRARY,
        VHDL_USE,
        VHDL_ARCHITECTURE,
        VHDL_OF,
        VHDL_IS,
        VHDL_BEGIN,
        VHDL_END,
        VHDL_ENTITY,
        VHDL_PROCESS,
        NUMBER_OF_KEYWORDS
      };
      
      enum Standard {
        VHDL_1987,
        VHDL_1993,
        VHDL_2002,
        VHDL_2008,
        NUMBER_OF_STANDARDS
      };
      
      struct KeywordInfo {
        Keyword keyword;
        char* text;
        Standard standard;
      };
      
      static KeywordInfo* getKeywordInfo() { 
        static KeywordInfo a[NUMBER_OF_KEYWORDS] = {
          {VHDL_LIBRARY, (char *)"library", VHDL_1987},
          {VHDL_USE, (char *)"use", VHDL_1987},
          {VHDL_ARCHITECTURE, (char *)"architecture", VHDL_1987},
          {VHDL_OF, (char *)"of", VHDL_1987},
          {VHDL_IS, (char *)"is", VHDL_1987},
          {VHDL_BEGIN, (char *)"begin", VHDL_1987},
          {VHDL_END, (char *)"end", VHDL_1987},
          {VHDL_ENTITY, (char *)"entity", VHDL_1987},
          {VHDL_PROCESS, (char *)"process", VHDL_1987}};
        return a;
      }
      
      int skipWhiteSpaceAndComments(::ast::TokenScanner<Scanner>* scanner);

    };

 
  }
}

#endif


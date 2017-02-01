
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

      static const int NUMBER_OF_SPECIAL_CHARACTERS = 12;

      static char* getSpecialCharacters() {
        static char a[NUMBER_OF_SPECIAL_CHARACTERS] =
          {'(', ')', '\'', '=', '>', '<', ';', '.', ':',',', '&', '+'};
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
        VHDL_TYPE,
        VHDL_RANGE,
        VHDL_TO,
        VHDL_DOWNTO,
        VHDL_VARIABLE,
        VHDL_REPORT,
        VHDL_SEVERITY,
	VHDL_IF,
	VHDL_THEN,
	VHDL_ELSIF,
	VHDL_ELSE,
	VHDL_FOR,
	VHDL_IN,
	VHDL_LOOP,
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
          {VHDL_PROCESS, (char *)"process", VHDL_1987},
          {VHDL_TYPE, (char *)"type", VHDL_1987},
          {VHDL_RANGE, (char *)"range", VHDL_1987},
          {VHDL_TO, (char *)"to", VHDL_1987},
          {VHDL_DOWNTO, (char *)"downto", VHDL_1987},
          {VHDL_VARIABLE, (char *)"variable", VHDL_1987},
          {VHDL_REPORT, (char *)"report", VHDL_1987},
          {VHDL_SEVERITY, (char *)"severity", VHDL_1987},
          {VHDL_IF, (char *)"if", VHDL_1987},
          {VHDL_THEN, (char *)"then", VHDL_1987},
          {VHDL_ELSIF, (char *)"elsif", VHDL_1987},
          {VHDL_ELSE, (char *)"else", VHDL_1987},
          {VHDL_FOR, (char *)"for", VHDL_1987},
          {VHDL_IN, (char *)"in", VHDL_1987},
          {VHDL_LOOP, (char *)"loop", VHDL_1987}};
        return a;
      }
      
      int skipWhiteSpaceAndComments(::ast::TokenScanner<Scanner>* scanner);

    };

 
  }
}

#endif


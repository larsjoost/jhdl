
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

      static const int NUMBER_OF_SPECIAL_CHARACTERS = 14;

      static char* getSpecialCharacters() {
        static char a[NUMBER_OF_SPECIAL_CHARACTERS] =
          {'(', ')', '\'', '=', '>', '<', ';', '.', ':',',', '&', '+', '/', '-'};
        return a;
      }
        
      static const bool CASE_SENSITIVE = false;
      
      enum Keyword {
        VHDL_LIBRARY,
        VHDL_USE,
        VHDL_PACKAGE,
        VHDL_BODY,
        VHDL_ARCHITECTURE,
        VHDL_OF,
        VHDL_IS,
        VHDL_BEGIN,
        VHDL_END,
        VHDL_ENTITY,
        VHDL_PROCESS,
        VHDL_TYPE,
        VHDL_SUBTYPE,
        VHDL_ARRAY,
        VHDL_RANGE,
        VHDL_TO,
        VHDL_DOWNTO,
        VHDL_UNITS,
        VHDL_VARIABLE,
        VHDL_REPORT,
        VHDL_SEVERITY,
	VHDL_IF,
	VHDL_THEN,
	VHDL_ELSIF,
	VHDL_ELSE,
        VHDL_WHEN,
	VHDL_FOR,
	VHDL_IN,
        VHDL_OUT,
        VHDL_INOUT,
        VHDL_BUFFER,
	VHDL_LOOP,
	VHDL_WAIT,
        VHDL_SIGNAL,
        VHDL_CONSTANT,
        VHDL_GENERATE,
        VHDL_BLOCK,
        VHDL_FUNCTION,
        VHDL_RETURN,
        VHDL_GENERIC,
        VHDL_PORT,
        VHDL_MAP,
        VHDL_NOT,
        VHDL_OR,
        VHDL_AND,
        VHDL_NOR,
        VHDL_NAND,
        VHDL_XOR,
        VHDL_XNOR,
        VHDL_ABS,
        VHDL_MOD,
        VHDL_REM,
        VHDL_SLL,
        VHDL_SRL,
        VHDL_SLA,
        VHDL_SRA,
        VHDL_ROL,
        VHDL_ROR,
        NUMBER_OF_KEYWORDS
      };
      
      struct KeywordInfo {
        Keyword keyword;
        char* text;
        int standard;
      };
      
      static KeywordInfo* getKeywordInfo() { 
        static KeywordInfo a[NUMBER_OF_KEYWORDS] = {
          {VHDL_LIBRARY, (char *)"library", 1987},
          {VHDL_USE, (char *)"use", 1987},
          {VHDL_PACKAGE, (char *)"package", 1987},
          {VHDL_BODY, (char *)"body", 1987},
          {VHDL_ARCHITECTURE, (char *)"architecture", 1987},
          {VHDL_OF, (char *)"of", 1987},
          {VHDL_IS, (char *)"is", 1987},
          {VHDL_BEGIN, (char *)"begin", 1987},
          {VHDL_END, (char *)"end", 1987},
          {VHDL_ENTITY, (char *)"entity", 1987},
          {VHDL_PROCESS, (char *)"process", 1987},
          {VHDL_TYPE, (char *)"type", 1987},
          {VHDL_SUBTYPE, (char *)"subtype", 1987},
          {VHDL_ARRAY, (char *)"array", 1987},
          {VHDL_RANGE, (char *)"range", 1987},
          {VHDL_TO, (char *)"to", 1987},
          {VHDL_DOWNTO, (char *)"downto", 1987},
          {VHDL_UNITS, (char *)"units", 1987},
          {VHDL_VARIABLE, (char *)"variable", 1987},
          {VHDL_REPORT, (char *)"report", 1987},
          {VHDL_SEVERITY, (char *)"severity", 1987},
          {VHDL_IF, (char *)"if", 1987},
          {VHDL_THEN, (char *)"then", 1987},
          {VHDL_ELSIF, (char *)"elsif", 1987},
          {VHDL_ELSE, (char *)"else", 1987},
          {VHDL_WHEN, (char *)"when", 1987},
          {VHDL_FOR, (char *)"for", 1987},
          {VHDL_IN, (char *)"in", 1987},
          {VHDL_OUT, (char *)"out", 1987},
          {VHDL_INOUT, (char *)"inout", 1987},
          {VHDL_BUFFER, (char *)"buffer", 1987},
          {VHDL_LOOP, (char *)"loop", 1987},
          {VHDL_WAIT, (char *)"wait", 1987},
          {VHDL_SIGNAL, (char *)"signal", 1987},
          {VHDL_CONSTANT, (char *)"constant", 1987},
          {VHDL_GENERATE, (char *)"generate", 1987},
          {VHDL_BLOCK, (char *)"block", 1987},
          {VHDL_FUNCTION, (char *)"function", 1987},
          {VHDL_RETURN, (char *)"return", 1987},
          {VHDL_GENERIC, (char *)"generic", 1987},
          {VHDL_PORT, (char *)"port", 1987},
          {VHDL_MAP, (char *)"map", 1987},
          {VHDL_NOT, (char *)"not", 1987},
          {VHDL_OR, (char *)"or", 1987},
          {VHDL_AND, (char *)"and", 1987},
          {VHDL_NOR, (char *)"nor", 1987},
          {VHDL_NAND, (char *)"nand", 1987},
          {VHDL_XOR, (char *)"xor", 1987},
          {VHDL_XNOR, (char *)"xnor", 1994},
          {VHDL_ABS, (char *)"abs", 1987},
          {VHDL_MOD, (char *)"mod", 1987},
          {VHDL_REM, (char *)"rem", 1987},
          {VHDL_SLL, (char *)"sll", 1993},
          {VHDL_SRL, (char *)"srl", 1993},
          {VHDL_SLA, (char *)"sla", 1993},
          {VHDL_SRA, (char *)"sra", 1993},
          {VHDL_ROL, (char *)"rol", 1993},
          {VHDL_ROR, (char *)"ror", 1993}};
        return a;
      }

      int skipWhiteSpaceAndComments(::ast::TokenScanner<Scanner>* scanner);

    };

 
  }
}

#endif



#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionOperator* ExpressionOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      {
        static const int CHARS_SIZE = 1;
        static const char* chars[CHARS_SIZE] = {"=>"};
        for (int i=0; i<CHARS_SIZE; i++) {
          if (scanner->match(chars[i])) {
            return this;
          }
        }
      }
      {
        static const int CHARS_SIZE = 12;
        static const char* chars[CHARS_SIZE] = {"/=", "/", ">=", ">", "<=", "<", "**", "*", "&", "+", "-", "="};
        for (int i=0; i<CHARS_SIZE; i++) {
          if (scanner->optional(chars[i])) {
            op = std::string(chars[i]);
            return this;
          }
        }
      }
      static const int KEYWORDS_SIZE = 14;
      static const scanner::Scanner::Keyword keywords[KEYWORDS_SIZE] = {
        scanner::Scanner::VHDL_AND, scanner::Scanner::VHDL_OR, scanner::Scanner::VHDL_NAND,
        scanner::Scanner::VHDL_NOR, scanner::Scanner::VHDL_XOR, scanner::Scanner::VHDL_XNOR,
        scanner::Scanner::VHDL_MOD, scanner::Scanner::VHDL_REM, scanner::Scanner::VHDL_SRL,
        scanner::Scanner::VHDL_SLL, scanner::Scanner::VHDL_SLA, scanner::Scanner::VHDL_SRA,
        scanner::Scanner::VHDL_ROL, scanner::Scanner::VHDL_ROR};
      for (int i=0; i<KEYWORDS_SIZE; i++) {
        if (scanner->optional(keywords[i])) {
          op = scanner->toString(keywords[i]);
          return this;
        }
      }
      return this;
    }

  }
}


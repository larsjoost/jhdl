
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionOperator* ExpressionOperator::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      static const int CHARS_SIZE = 11;
      static const char* chars[CHARS_SIZE] = {"/=", ">=", ">", "<=", "<", "**", "*", "&", "+", "-", "="};
      for (int i=0; i<CHARS_SIZE; i++) {
        if (scanner->optional(chars[i])) {
          op = std::string(chars[i]);
          return this;
        }
      }
      static const int KEYWORDS_SIZE = 7;
      static const scanner::Scanner::Keyword keywords[KEYWORDS_SIZE] = {
        scanner::Scanner::VHDL_AND, scanner::Scanner::VHDL_OR, scanner::Scanner::VHDL_NAND,
        scanner::Scanner::VHDL_NOR, scanner::Scanner::VHDL_XOR, scanner::Scanner::VHDL_XNOR,
        scanner::Scanner::VHDL_MOD};
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


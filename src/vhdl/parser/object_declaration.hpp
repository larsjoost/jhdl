
#ifndef VHDL_PARSER_OBJECTDECLARATION_H_
#define VHDL_PARSER_OBJECTDECLARATION_H_

#include <cassert>

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/object_declaration.hpp"
#include "basic_identifier.hpp"
#include "declaration_initialization.hpp"
#include "subtype_indication.hpp"

namespace vhdl {
  namespace parser {

   template<scanner::Scanner::Keyword keyword, bool defaultType = false>
    class ObjectDeclaration :
      public ::ast::ObjectDeclaration {

    public:

      ObjectDeclaration<keyword, defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        if (defaultType) {
          scanner->optional(keyword);
        } else {
          scanner->accept(keyword);
        }
        switch (keyword) {
          case scanner::Scanner::VHDL_CONSTANT: objectType = CONSTANT; break;
          case scanner::Scanner::VHDL_SIGNAL: objectType = SIGNAL; break;
          case scanner::Scanner::VHDL_VARIABLE: objectType = VARIABLE; break;
          default: assert(false);
        }
        identifier = scanner->expect<BasicIdentifier>();
        scanner->expect(":");
        static const int NUMBER_OF_DIRECTIONS = 4;
        static const scanner::Scanner::Keyword DIRECTIONS[NUMBER_OF_DIRECTIONS] =
          {scanner::Scanner::VHDL_IN,
           scanner::Scanner::VHDL_OUT,
           scanner::Scanner::VHDL_INOUT,
           scanner::Scanner::VHDL_BUFFER};
        static const ast::ObjectDeclaration::Direction TRANSLATE[NUMBER_OF_DIRECTIONS] =
          {
            ast::ObjectDeclaration::IN,
            ast::ObjectDeclaration::OUT,
            ast::ObjectDeclaration::INOUT,
            ast::ObjectDeclaration::BUFFER
          };
        for (int i=0; i<NUMBER_OF_DIRECTIONS; i++) {
          if (scanner->optional(DIRECTIONS[i])) {
            direction = TRANSLATE[i];
            break;
          }
        }
        type = scanner->expect<BasicIdentifier>();
        subtype = scanner->optional<SubtypeIndication>();
        initialization = scanner->optional<DeclarationInitialization>();
        return this;
      }

    };

  }

}
  

#endif

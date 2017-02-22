
#include "design_file.hpp"
#include "../ast/scanner.hpp"
#include "../ast/text.hpp"
#include "../vhdl/scanner/scanner.hpp"
#include "../vhdl/parser/design_unit.hpp"
#include "../verilog/scanner/scanner.hpp"
#include "../verilog/parser/design_unit.hpp"

namespace parser {

  DesignFile::DesignFile(const char* f, bool verbose) {
    filename = f;
    if (filename == NULL) {
      fprintf(stderr, "File name is not specified\n");
      throw FileNotFound();
    } else {
      try {
        std::string s = std::string(f);
        if (s.substr(s.find_last_of(".") + 1) == "v") {
          ::ast::Scanner<verilog::scanner::Scanner> scanner =
            ::ast::Scanner<verilog::scanner::Scanner>(verbose);
          scanner.loadFile(filename);
          while (true) {
            designUnits.add(scanner.expect<verilog::parser::DesignUnit>());
          }
        } else {
          ::ast::Scanner<vhdl::scanner::Scanner> scanner =
            ::ast::Scanner<vhdl::scanner::Scanner>(verbose);
          scanner.loadFile(filename);
          while (true) {
            designUnits.add(scanner.expect<vhdl::parser::DesignUnit>());
          }
        }
      } catch (ast::TokenEof e) {
      }
    }
  } 

}

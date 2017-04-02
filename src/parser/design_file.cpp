
#include "design_file.hpp"
#include "../ast/text.hpp"
#include "../vhdl/parser/design_unit.hpp"
#include "../verilog/parser/design_unit.hpp"

namespace parser {

  DesignFile::DesignFile(bool verbose) : verbose(verbose) {}

  void DesignFile::parse(std::string& filename) {
    this->filename = filename;
    try {
      std::string s = filename;
      if (s.substr(s.find_last_of(".") + 1) == "v") {
        verilogScanner.loadFile(filename);
        verilogScanner.setVerbose(verbose);
        while (true) {
          designUnits.add(verilogScanner.expect<verilog::parser::DesignUnit>());
        }
      } else {
        vhdlScanner.loadFile(filename);
        vhdlScanner.setVerbose(verbose);
        while (true) {
          designUnits.add(vhdlScanner.expect<vhdl::parser::DesignUnit>());
        }
      }
    } catch (ast::TokenEof e) {
    }
  }

}

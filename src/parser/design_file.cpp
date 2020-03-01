
#include "design_file.hpp"
#include "../ast/text.hpp"
#include "../vhdl/parser/design_unit.hpp"
#include "../verilog/parser/design_unit.hpp"

namespace parser {

  void DesignFile::parse(std::string& filename) {
    m_debug.functionStart("parse");
    this->filename = filename;
    try {
      std::string s = filename;
      if (s.substr(s.find_last_of(".") + 1) == "v") {
        a_language_type = LanguageType::VERILOG;
        verilogScanner.loadFile(filename);
        while (true) {
          ast::DesignUnit* design_unit = verilogScanner.expect<verilog::parser::DesignUnit, ast::DesignUnit>();
          m_debug.debug("Design unit " + std::string(design_unit ? "added" : "not found"));
          designUnits.add(design_unit);
        }
      } else {
        a_language_type = LanguageType::VHDL;
        vhdlScanner.loadFile(filename);
        while (true) {
          designUnits.add(vhdlScanner.expect<vhdl::parser::DesignUnit, ast::DesignUnit>());
        }
      }
    } catch (ast::TokenEof e) {
      m_debug.debug("Caught TokenEof");
    }
    m_debug.functionEnd("parse");
  }

}


#include "design_file.hpp"
#include "../ast/scanner.hpp"
#include "../ast/text.hpp"
#include "../vhdl/scanner/scanner.hpp"
#include "../vhdl/parser/design_file.hpp"

namespace parser {

  DesignFile::DesignFile(const char* f, int verbose) {
    filename = f;
    if (filename == NULL) {
      fprintf(stderr, "File name is not specified\n");
      throw FileNotFound();
    } else {
      try {
	vhdl::scanner::Scanner scanner = vhdl::scanner::Scanner(verbose);
	scanner.loadFile(filename);
	scanner.optional<vhdl::parser::DesignFile>();
      } catch (ast::TextEof e) {
      }
    }
  }

}
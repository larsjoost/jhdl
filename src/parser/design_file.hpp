#include "../ast/scanner.hpp"
#include "../ast/design_file.hpp"
#include "../verilog/scanner/scanner.hpp"
#include "../vhdl/scanner/scanner.hpp"

#include "../debug/debug.hpp"

namespace parser {

  class FileNotFound {};
  
  class DesignFile : public ::ast::DesignFile {

    Debug<false> m_debug;
    
    ::ast::Scanner<verilog::scanner::Scanner> verilogScanner;
    ::ast::Scanner<vhdl::scanner::Scanner> vhdlScanner;

  public:

    DesignFile() : m_debug(this) {};

    void parse(std::string& filename);

  };

}

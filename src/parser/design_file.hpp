#include "../ast/scanner.hpp"
#include "../ast/design_file.hpp"
#include "../verilog/scanner/scanner.hpp"
#include "../vhdl/scanner/scanner.hpp"

namespace parser {

  class FileNotFound {};
  
  class DesignFile :
    public ::ast::DesignFile {

    ::ast::Scanner<verilog::scanner::Scanner> verilogScanner;
    ::ast::Scanner<vhdl::scanner::Scanner> vhdlScanner;

    bool verbose = false;
    
  public:
    
    DesignFile(bool verbose = false);

    void parse(std::string& filename);

  };

}

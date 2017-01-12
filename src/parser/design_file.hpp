
#include "../ast/design_file.hpp"

namespace parser {

  class FileNotFound {};
  
  class DesignFile :
    public ::ast::DesignFile {

  public:
    
    DesignFile (const char* filename, int verbose = 0);

  };

}


namespace parser {

  class FileNotFound {};
  
  class DesignFile {

  public:
    const char* filename;
    
    DesignFile (const char* filename, int verbose = 0);

  };

}

#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include <string>
#include <fstream>

#include "../ast/object_type.hpp"

#include "../debug/debug.hpp"

namespace generator {

  class parameters {

    struct FileInfo {
      std::string fileName;
      std::ofstream outputFile;
      int indent = 0;
      int line_number = 1;
    };

    FileInfo sourceFileInfo;
    FileInfo headerFileInfo;

    Debug debug = Debug("parameters", false);

    bool quiet = false;
    
    void open(FileInfo& fileInfo, std::string& filename, std::string extension);

    FileInfo& getFileInfo();
    
  public:
    enum FileSelect {HEADER_FILE, SOURCE_FILE};
  private:
    FileSelect a_file_select;
  public:
    enum Area {DECLARATION, INITIALIZATION, IMPLEMENTATION, NONE, AREA_SIZE};
    int index;
    Area area;
    bool isArea(Area a);
    void setArea(Area a);
    void printArea(std::string name);
    ast::ObjectValueContainer returnType;
    void incIndent();
    void decIndent();
    void open(std::string filename);
    void close();
    void println(int position, std::string message);
    void println(std::string message);
    void println(Area a, std::string message);
    FileSelect selectFile(FileSelect s);
    bool isQuiet();
    bool setQuiet(bool quiet);
    std::string getFileName(FileSelect s); 
    std::string replaceFileExtension(std::string filename, std::string extension);
    std::string baseName(std::string filename);
 };

}

#endif

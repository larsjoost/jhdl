#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include <string>
#include <fstream>

#include "../ast/object_type.hpp"

namespace generator {

  class parameters {

    struct FileInfo {
      std::string fileName;
      std::ofstream outputFile;
      int indent = 0;
    };

    FileInfo* fileInfo = NULL;
    FileInfo* previousFileInfo;
    FileInfo sourceFileInfo;
    FileInfo headerFileInfo;

    void open(FileInfo& fileInfo, std::string& filename, std::string extension);


  public:
    enum FileSelect {HEADER_FILE, SOURCE_FILE};
    ast::ObjectValueContainer returnType;
    void incIndent();
    void decIndent();
    void open(std::string filename);
    void close();
    void println(std::string message);
    void selectFile(FileSelect s);
    void revertSelectFile();
    bool isQuiet();
    std::string getFileName(FileSelect s); 
    std::string replaceFileExtension(std::string filename, std::string extension);
    std::string baseName(std::string filename);
 };

}

#endif

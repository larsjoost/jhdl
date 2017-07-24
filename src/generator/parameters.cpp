
#include "parameters.hpp"

namespace generator {

  void parameters::incIndent() {
    if (fileInfo) {
      fileInfo->indent += 2;
    }
  }

  void parameters::decIndent() {
    if (fileInfo) {
      fileInfo->indent -= 2;
    }
  }

  std::string parameters::replaceFileExtension(std::string filename, std::string extension) {
    return baseName(filename) + extension;
  }

  std::string parameters::baseName(std::string filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
  }
  
  void parameters::open(FileInfo& f, std::string& filename, std::string extension) {
    f.fileName = replaceFileExtension(filename, extension); 
    f.outputFile.open(f.fileName);
  };
  
  void parameters::open(std::string filename) {
    open(headerFileInfo, filename, ".hpp");
    open(sourceFileInfo, filename, ".cpp");
    selectFile(HEADER_FILE);
  }
  
  void parameters::close() {
    headerFileInfo.outputFile.close();
    sourceFileInfo.outputFile.close();
  }
  
  void parameters::selectFile(FileSelect s) {
    previousFileInfo = fileInfo;
    if (s == HEADER_FILE) {
      fileInfo = &headerFileInfo;
    } else {
      fileInfo = &sourceFileInfo;
    }
  }

  void parameters::revertSelectFile() {
    fileInfo= previousFileInfo;
  }

  void parameters::println(int position, std::string text) {
    if (fileInfo) {
      fileInfo->outputFile << std::string(position, ' ') << text << std::endl;
    }
  }

  void parameters::println(std::string text) {
    if (fileInfo) {
      println(fileInfo->indent, text);
    }
  }

  bool parameters::isArea(Area a) {
    return a == area;
  }
  
  void parameters::println(Area a, std::string text) {
    if (isArea(a)) {
      println(text);
    }
  }

  bool parameters::isQuiet() {
    return (fileInfo ? true : false);
  }
  
  std::string parameters::getFileName(FileSelect s) {
    selectFile(s);
    std::string name = fileInfo->fileName;
    revertSelectFile();
    return name;
  }


}


#include "parameters.hpp"

namespace generator {

  parameters::FileInfo& parameters::getFileInfo() {
    if (a_file_select == HEADER_FILE) {
      return headerFileInfo;
    } 
    return sourceFileInfo;    
  }
  
  void parameters::incIndent() {
    getFileInfo().indent += 2;
  }

  void parameters::decIndent() {
    getFileInfo().indent -= 2;
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
  
  parameters::FileSelect parameters::selectFile(FileSelect file_select) {
    FileSelect a = a_file_select;
    a_file_select = file_select;
    return a;
  }

  void parameters::println(int position, std::string text) {
    if (!isQuiet()) {
      getFileInfo().outputFile << std::string(position, ' ') << text << std::endl;
    }
  }

  void parameters::println(std::string text) {
    println(getFileInfo().indent, text);
  }

  bool parameters::isArea(Area a) {
    return a == area;
  }
  
  void parameters::setArea(Area a) {
    index = 1;
    area = a;
  }

  void parameters::printArea(std::string name) {
    const static std::string translate[AREA_SIZE] = {"declaration", "initialization", "implementation", "none"};
    println("// " + name + " area: " + translate[area]);
  }
  
  void parameters::println(Area a, std::string text) {
    if (isArea(a)) {
      println(text);
    }
  }

  bool parameters::isQuiet() {
    return quiet;
  }

  bool parameters::setQuiet(bool quiet) {
    bool q = this->quiet;
    this->quiet = quiet;
    return q;
  }

  std::string parameters::getFileName(FileSelect file_select) {
    FileSelect tmp_file_select = selectFile(file_select);
    std::string name = getFileInfo().fileName;
    selectFile(tmp_file_select);
    return name;
  }


}

#include <cassert>

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
      std::string indent = std::string(position, ' ');
      getFileInfo().outputFile << indent << text << std::endl;
      std::string line = std::to_string(getFileInfo().line_number);
      int spaces = 3 - line.size();
      if (spaces > 0) {
        line = std::string(spaces, ' ') + line;
      }
      debug.debug("line " + line + ": " + indent + text, true, Output::Color::BLUE);
      getFileInfo().line_number++;
    }
  }

  void parameters::println(std::string text) {
    println(getFileInfo().indent, text);
  }

  std::string parameters::AreaToString(Area a) {
    switch (a) {
    case Area::CONSTRUCTOR: return "constructor";
    case Area::DECLARATION: return "declaration";
    case Area::INITIALIZATION: return "initialization";
    case Area::IMPLEMENTATION: return "implementation";
    case Area::INTERFACE: return "interface";
    case Area::NONE: return "none";
    }
    assert(false);
  }
  
  int parameters::ConvertInteger(Area a) {
    return static_cast<std::underlying_type_t<Area>>(a);
  }
  
  void parameters::println(Area a, std::string text, int position) {
    int index = ConvertInteger(a);
    auto lines = printlines.find(index);
    if (lines != printlines.end()) {
      int indent = position < 0 ? lines->second.indent : position;
      lines->second.lines.push_back({indent, text});
    } else {
      AreaInfo l;
      int indent = position < 0 ? 0 : position;
      l.lines.push_back({indent, text});
      printlines[index] = l;
    }
  }

  void parameters::Flush(Area a) {
    int index = ConvertInteger(a);
    auto lines = printlines.find(index);
    if (lines != printlines.end()) {
      while (!lines->second.lines.empty()) {
        auto& lineInfo = lines->second.lines.back();
        println(lineInfo.text);
        lines->second.lines.pop_back();
      }
    }
  }
  
  bool parameters::PrintlnBuffersEmpty() {
    bool result = true;
    for (auto& i : printlines) {
      if (!i.second.lines.empty()) {
        result = false;
        std::cerr << "Buffer " << i.first << " is not empty" << std::endl;
        break;
      }
    }
    return result;
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

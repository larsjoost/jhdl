#include <cassert>

#include "parameters.hpp"

namespace generator {

  parameters::FileInfo& parameters::getFileInfo() {
    if (a_file_select == HEADER_FILE) {
      return headerFileInfo;
    } 
    return sourceFileInfo;    
  }

  void parameters::incIndent(Area a) {
    auto f = [&](AreaInfo& info) {
      info.indent += 2;
    };
    AccessAreaInfo(a, f);
  }

  void parameters::incIndent() {
    getFileInfo().indent += 2;
  }

  void parameters::decIndent(Area a) {
    auto f = [&](AreaInfo& info) {
      info.indent -= 2;
    };
    AccessAreaInfo(a, f);
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

  void parameters::println(std::string text, int position) {
    if (!isQuiet()) {
      if (position < 0) { position = getFileInfo().indent; }
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

  std::string parameters::AreaToString(Area a) {
    switch (a) {
    case Area::INITIALIZER_LIST: return "initializer list";
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
    if (!isQuiet()) {
      Area current_area = GetArea();
      assert(current_area != Area::NONE);
      if (a == current_area) {
        println(text, position);
      } else {
        debug.debug("[" + AreaToString(GetArea()) + ", " + AreaToString(a) + "]: " + text, true, Output::Color::GREEN);
        auto f = [&](AreaInfo& info) {
          int indent = position < 0 ? 0 : position;
          info.lines.push_front({indent, text});
        };
        AccessAreaInfo(a, f);
      }
    }
  }

  void parameters::Flush(Area a) {
    debug.functionStart("Flush(area = " + AreaToString(a) + ")");
    if (verbose) {println("// Flush area(" + std::to_string(printlines.size()) + ") " + AreaToString(a), getFileInfo().indent);}
    auto f = [&](AreaInfo& info) {
      while (!info.lines.empty()) {
        auto& lineInfo = info.lines.back();
        println(lineInfo.text, info.indent + getFileInfo().indent);
        info.lines.pop_back();
      }
    };
    AccessAreaInfo(a, f);
    debug.functionEnd("Flush");
  }
  
  void parameters::DescendHierarchy(Area area) {
    printlines.push_back({area, AreaInfoMap()});
    if (verbose) {println("// Descend Hierarchy (" + std::to_string(printlines.size()) + ") " + AreaToString(area), getFileInfo().indent);}
  }

  void parameters::AscendHierarchy() {
    if (verbose) {println("// Ascend Hierarchy (" + std::to_string(printlines.size() + 1) + ")", getFileInfo().indent);}
    bool ok = true;
    for (auto& i : printlines.back().map) {
      if (!i.second.lines.empty()) {
        std::cerr << "Buffer " << i.first << " is not empty" << std::endl;
        std::cerr << "Contents of buffer " << i.first << ":" << std::endl;
        for (auto& x : i.second.lines) {
          std::cerr << x.text << std::endl;
        }
        ok = false;
      }
    }
    assert(ok);
    printlines.pop_back();
  }

  parameters::Area parameters::SetArea(Area area, bool flush) {
    debug.functionStart("SetArea(area = " + AreaToString(area) + ")");
    if (verbose) {println("// Set area(" + std::to_string(printlines.size()) + ") " + AreaToString(area), getFileInfo().indent);}
    Areas& areas = printlines.back();
    Area a = areas.area;
    areas.area = area;
    if (flush) {Flush(area);};
    debug.functionEnd("SetArea = " + AreaToString(a));
    return a;
  };

  parameters::Area parameters::GetArea() {
    Areas& a = printlines.back();
    return a.area;
  };

  bool parameters::IsArea(Area area) {
    return area == GetArea();
  }
  
  std::string parameters::ToList(Area a) {
    std::string result;
    std::string delimiter;
    auto f = [&](AreaInfo& info) {
      while (!info.lines.empty()) {
        auto& lineInfo = info.lines.back();
        result += delimiter + lineInfo.text;
        delimiter = ", ";
        info.lines.pop_back();
      }
    };
    AccessAreaInfo(a, f);
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

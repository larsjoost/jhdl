#include <cassert>

#include "parameters.hpp"

namespace generator {

  parameters::FileInfo& parameters::getFileInfo() {
    if (a_file_select == FileSelect::HEADER) {
      return headerFileInfo;
    } 
    return sourceFileInfo;    
  }

  parameters::AreasHierarchy& parameters::GetAreas() {
    return getFileInfo().printlines;
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
    debug.functionStart("open(filename = " + filename + ")");
    open(headerFileInfo, filename, ".hpp");
    open(sourceFileInfo, filename, ".cpp");
    selectFile(FileSelect::HEADER);
    debug.functionEnd("open");
  }
  
  void parameters::close() {
    debug.functionStart("close");
    selectFile(FileSelect::HEADER);
    AscendHierarchy();
    headerFileInfo.outputFile.close();
    selectFile(FileSelect::SOURCE);
    AscendHierarchy();
    sourceFileInfo.outputFile.close();
    debug.functionEnd("close");
  }
  
  parameters::FileSelect parameters::selectFile(FileSelect file_select) {
    debug.functionStart("selectFile(file_select = " + ToString(file_select) + ")");
    FileSelect a = a_file_select;
    a_file_select = file_select;
    debug.functionEnd("selectFile");
    return a;
  }

  void parameters::write(const std::string& text) {
    getFileInfo().outputFile << text << std::endl;
    std::string line = std::to_string(getFileInfo().line_number);
    debug.debug("line " + line + ": " + text, true, Output::Color::BLUE);
    getFileInfo().line_number++;
  }

  void parameters::println(std::string text) {
    if (!isQuiet()) {
      Areas& a = GetAreas().front();
      a.buffer.push_back(text);
    }
  }

  void parameters::println(Area a, std::string text) {
    if (!isQuiet()) {
      debug.functionStart("println");
      Area current_area = GetArea();
      assert(current_area != Area::NONE);
      if (a == current_area) {
        println(text);
      } else {
        debug.debug("[" + ToString(GetArea()) + ", " + ToString(a) + "]: " + text, true, Output::Color::GREEN);
        auto f = [&](Areas& areas, AreaInfo& info) {
          info.lines.push_back(text);
        };
        AccessAreaInfo(a, f);
      }
      debug.functionEnd("println");
    }
  }

  std::string parameters::ToString(FileSelect f) {
    switch (f) {
    case FileSelect::HEADER: return "HEADER";
    case FileSelect::SOURCE: return "SOURCE";
    }
    assert(false);
  }
  
  std::string parameters::ToString(Area a) {
    switch (a) {
    case Area::TOP: return "top";
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
  
  void parameters::Flush(Area a) {
    debug.functionStart("Flush(area = " + ToString(a) + ")");
    auto f = [&](Areas& areas, AreaInfo& info) {
      while (!info.lines.empty()) {
        std::string& line = info.lines.front();
        areas.buffer.push_back(line);
        info.lines.pop_front();
      }
    };
    AccessAreaInfo(a, f);
    debug.functionEnd("Flush");
  }
  
  void parameters::DescendHierarchy(Area area) {
    GetAreas().push_front({area, AreaInfoMap()});
    if (verbose) {println("// Descend Hierarchy (" + std::to_string(GetAreas().size()) + ") " + ToString(area));}
  }

  void parameters::AscendHierarchy() {
    if (verbose) {println("// Ascend Hierarchy (" + std::to_string(GetAreas().size() + 1) + ")");}
    bool ok = true;
    Areas& current_area = GetAreas().front();
    Area area = current_area.area;
    if (GetAreas().size() > 1) {
      auto f = [&](Areas& areas, AreaInfo& info) {
        while (!current_area.buffer.empty()) {
          info.lines.push_back(current_area.buffer.front());
          current_area.buffer.pop_front();
        }
      };
      AccessAreaInfo(area, f, true);
    } else {
      while(!current_area.buffer.empty()) {
        write(current_area.buffer.front());
        current_area.buffer.pop_front();
      }
    }
    GetAreas().pop_front();
    if (area == GetArea() && GetAreas().size() > 0) {
      Flush(area);
    }
  }

  parameters::Area parameters::SetArea(Area area) {
    debug.functionStart("SetArea(area = " + ToString(area) + ")");
    debug.debug("File select = " + ToString(a_file_select));
    if (verbose) {println("// Set area(" + std::to_string(GetAreas().size()) + ") " + ToString(area));}
    Areas& areas = GetAreas().front();
    Area a = areas.area;
    areas.area = area;
    Flush(area);
    debug.functionEnd("SetArea = " + ToString(a));
    return a;
  };

  parameters::Area parameters::GetArea() {
    Areas& a = GetAreas().front();
    return a.area;
  };

  bool parameters::IsArea(Area area) {
    return area == GetArea();
  }
  
  std::string parameters::ToList(Area a) {
    std::string result;
    std::string delimiter;
    auto f = [&](Areas& areas, AreaInfo& info) {
      while (!info.lines.empty()) {
        std::string& text = info.lines.front();
        result += delimiter + text;
        delimiter = ", ";
        info.lines.pop_front();
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

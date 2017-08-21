#include <cassert>

#include "parameters.hpp"

namespace generator {

  parameters::FileInfo& parameters::getFileInfo() {
    if (a_file_select == HEADER_FILE) {
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
    open(headerFileInfo, filename, ".hpp");
    open(sourceFileInfo, filename, ".cpp");
    selectFile(HEADER_FILE);
  }
  
  void parameters::close() {
    selectFile(HEADER_FILE);
    AscendHierarchy();
    headerFileInfo.outputFile.close();
    selectFile(SOURCE_FILE);
    AscendHierarchy();
    sourceFileInfo.outputFile.close();
  }
  
  parameters::FileSelect parameters::selectFile(FileSelect file_select) {
    FileSelect a = a_file_select;
    a_file_select = file_select;
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
      Area current_area = GetArea();
      assert(current_area != Area::NONE);
      if (a == current_area) {
        println(text);
      } else {
        debug.debug("[" + AreaToString(GetArea()) + ", " + AreaToString(a) + "]: " + text, true, Output::Color::GREEN);
        auto f = [&](Areas& areas, std::list<std::string>& info) {
          info.push_back(text);
        };
        AccessAreaInfo(a, f);
      }
    }
  }

  std::string parameters::AreaToString(Area a) {
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
    debug.functionStart("Flush(area = " + AreaToString(a) + ")");
    auto f = [&](Areas& areas, std::list<std::string>& info) {
      while (!info.empty()) {
        std::string& line = info.front();
        areas.buffer.push_back(line);
        info.pop_front();
      }
    };
    AccessAreaInfo(a, f);
    debug.functionEnd("Flush");
  }
  
  void parameters::DescendHierarchy(Area area) {
    GetAreas().push_front({area, AreaInfoMap()});
    if (verbose) {println("// Descend Hierarchy (" + std::to_string(GetAreas().size()) + ") " + AreaToString(area));}
  }

  void parameters::AscendHierarchy() {
    if (verbose) {println("// Ascend Hierarchy (" + std::to_string(GetAreas().size() + 1) + ")");}
    bool ok = true;
    Areas& current_area = GetAreas().front();
    for (auto& i : current_area.map) {
      if (!i.second.empty()) {
        std::cerr << "Hierarchy " << GetAreas().size() << " Buffer " << i.first << " is not empty" << std::endl;
        std::cerr << "Contents of buffer " << i.first << ":" << std::endl;
        for (auto& x : i.second) {
          std::cerr << x << std::endl;
        }
        ok = false;
      }
    }
    assert(ok);
    Area area = current_area.area;
    if (GetAreas().size() > 1) {
      auto f = [&](Areas& areas, std::list<std::string>& info) {
        while (!current_area.buffer.empty()) {
          info.push_back(current_area.buffer.front());
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
    if (area == GetArea()) {
      Flush(area);
    }
  }

  parameters::Area parameters::SetArea(Area area) {
    debug.functionStart("SetArea(area = " + AreaToString(area) + ")");
    if (verbose) {println("// Set area(" + std::to_string(GetAreas().size()) + ") " + AreaToString(area));}
    Areas& areas = GetAreas().front();
    Area a = areas.area;
    areas.area = area;
    Flush(area);
    debug.functionEnd("SetArea = " + AreaToString(a));
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
    auto f = [&](Areas& areas, std::list<std::string>& info) {
      while (!info.empty()) {
        std::string& text = info.front();
        result += delimiter + text;
        delimiter = ", ";
        info.pop_front();
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

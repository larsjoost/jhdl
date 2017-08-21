#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include "../debug/debug.hpp"

#include "../ast/object_type.hpp"

#include <string>
#include <fstream>
#include <unordered_map>

namespace generator {

  class parameters {

    Debug<true> debug;

    bool verbose = true;
    
  public:
    
    enum FileSelect {HEADER_FILE, SOURCE_FILE};
    enum class Area {TOP, INITIALIZER_LIST, CONSTRUCTOR, DECLARATION, INITIALIZATION, IMPLEMENTATION, INTERFACE, NONE};

  private:
    
    struct LineInfo {
      int indent;
      std::string text;
    };

    struct AreaInfo {
      int indent = 0;
      std::list<LineInfo> lines;
    };

    typedef std::unordered_map<int, AreaInfo> AreaInfoMap;

    struct Areas {
      Area area;
      AreaInfoMap map;
      int indent = 0;
      std::list<LineInfo> buffer;
    };
    
    typedef std::list<Areas> AreasHierarchy;
    
    struct FileInfo {
      std::string fileName;
      std::ofstream outputFile;
      int indent = 0;
      int line_number = 1;
      AreasHierarchy printlines;
    };

    FileInfo sourceFileInfo;
    FileInfo headerFileInfo;

    bool quiet = false;
    
    void open(FileInfo& fileInfo, std::string& filename, std::string extension);
    void write(const LineInfo& line_info);

    FileInfo& getFileInfo();

    AreasHierarchy& GetAreas();
    
    int ConvertInteger(Area a);
    FileSelect a_file_select;
    std::string AreaToString(Area a);

    template <typename Func>
    void AccessAreaInfo(Area a, Func func, bool second_element = false) {
      int index = ConvertInteger(a);
      auto areas = GetAreas().begin();
      if (second_element) {
        areas = std::next(areas);
      }
      auto lines = areas->map.find(index);
      if (lines != areas->map.end()) {
        func(*areas, lines->second);
      } else {
        AreaInfo l;
        AreaInfoMap& m = GetAreas().front().map;
        m[index] = l;
        func(*areas, l);
      }
    }

    void Flush(Area a);

  public:
    parameters() : debug("parameters") {
      selectFile(SOURCE_FILE);
      DescendHierarchy(Area::TOP);
      selectFile(HEADER_FILE);
      DescendHierarchy(Area::TOP);
    };
    int index;
    ast::ObjectValueContainer returnType;
    void incIndent();
    void incIndent(Area area);
    void decIndent();
    void decIndent(Area area);
    void DescendHierarchy(Area area);
    void AscendHierarchy();
    void open(std::string filename);
    void close();
    void println(std::string message, int position = -1);
    void println(Area a, std::string message, int position = -1);
    Area SetArea(Area area);
    Area GetArea();
    bool IsArea(Area area);
    std::string ToList(Area a);
    FileSelect selectFile(FileSelect s);
    bool isFile(FileSelect s) {return s == a_file_select; };
    bool isQuiet();
    bool setQuiet(bool quiet);
    std::string getFileName(FileSelect s); 
    std::string replaceFileExtension(std::string filename, std::string extension);
    std::string baseName(std::string filename);
 };

}

#endif

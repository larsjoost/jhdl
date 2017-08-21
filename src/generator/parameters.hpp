#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include "../debug/debug.hpp"

#include "../ast/object_type.hpp"

#include <string>
#include <fstream>
#include <unordered_map>

namespace generator {

  class parameters {

    Debug<false> debug;

    bool verbose = false;
    
  public:
    
    enum FileSelect {HEADER_FILE, SOURCE_FILE};
    enum class Area {TOP, INITIALIZER_LIST, CONSTRUCTOR, DECLARATION, INITIALIZATION, IMPLEMENTATION, INTERFACE, NONE};

  private:
    
    typedef std::unordered_map<int, std::list<std::string>> AreaInfoMap;

    struct Areas {
      Area area;
      AreaInfoMap map;
      std::list<std::string> buffer;
    };
    
    typedef std::list<Areas> AreasHierarchy;
    
    struct FileInfo {
      std::string fileName;
      std::ofstream outputFile;
      int line_number = 1;
      AreasHierarchy printlines;
    };

    FileInfo sourceFileInfo;
    FileInfo headerFileInfo;

    bool quiet = false;
    
    void open(FileInfo& fileInfo, std::string& filename, std::string extension);
    void write(const std::string& line);

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
        std::list<std::string> l;
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
    void DescendHierarchy(Area area);
    void AscendHierarchy();
    void open(std::string filename);
    void close();
    void println(std::string message);
    void println(Area a, std::string message);
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

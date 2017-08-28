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

    bool verbose = false;
    
  public:
    
    enum class FileSelect {HEADER, SOURCE};
    enum class Area {TOP, INITIALIZER_LIST, CONSTRUCTOR, DECLARATION, INITIALIZATION, IMPLEMENTATION, INTERFACE, NONE};

  private:

    struct AreaInfo {
      FileSelect a_file_select;
      Area a_area;
      parameters* a_parm;
      std::list<std::string> lines;
      ~AreaInfo() {
        if (!lines.empty()) {
          std::cerr <<
            "AreaInfo(" << a_parm->GetAreas().size() << ") " <<
            a_parm->ToString(a_area) << " of file " <<
            a_parm->ToString(a_file_select) << " not empty" << std::endl;
          std::cerr << "<Lines>" << std::endl;
          while(!lines.empty()) {
            std::cerr << lines.front() << std::endl;
            lines.pop_front();
          }
          std::cerr << "</Lines>" << std::endl;
          assert(false);
        }
      }
    };
    
    typedef std::unordered_map<int, AreaInfo> AreaInfoMap;

    struct Areas {
      Area area;
      AreaInfoMap map;
      std::list<std::string> buffer;
      ~Areas() {
        if (!buffer.empty()) {
          std::cerr << "Buffer is not empty. Contents:" << std::endl;
          for (auto& i : buffer) {
            std::cerr << i << std::endl;
          }
          assert(false);
        }
      }
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
    std::string ToString(Area a);
    std::string ToString(FileSelect f);
    
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
        l.a_file_select = a_file_select;
        l.a_area = a;
        l.a_parm = this;
        AreaInfoMap& m = GetAreas().front().map;
        m[index] = l;
        func(*areas, m[index]);
      }
    }

  public:
    parameters() : debug("parameters") {
      selectFile(FileSelect::SOURCE);
      DescendHierarchy(Area::TOP);
      selectFile(FileSelect::HEADER);
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
    void Flush(Area a);
    void Flush();
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

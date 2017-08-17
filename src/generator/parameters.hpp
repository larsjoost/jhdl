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
    
    struct FileInfo {
      std::string fileName;
      std::ofstream outputFile;
      int indent = 0;
      int line_number = 1;
    };

    FileInfo sourceFileInfo;
    FileInfo headerFileInfo;

    bool quiet = false;
    
    void open(FileInfo& fileInfo, std::string& filename, std::string extension);

    FileInfo& getFileInfo();

    struct LineInfo {
      int indent;
      std::string text;
    };
    
    struct AreaInfo {
      int indent = 0;
      std::list<LineInfo> lines;
    };
    
    std::unordered_map<int, AreaInfo> printlines;
    
  public:
    enum FileSelect {HEADER_FILE, SOURCE_FILE};
    enum class Area {INITIALIZER_LIST, CONSTRUCTOR, DECLARATION, INITIALIZATION, IMPLEMENTATION, INTERFACE, NONE};
  private:
    Area a_area;
    int ConvertInteger(Area a);
    FileSelect a_file_select;
    std::string AreaToString(Area a);
  public:
    parameters() : debug("parameters") {};
    int index;
    ast::ObjectValueContainer returnType;
    void incIndent();
    void decIndent();
    void open(std::string filename);
    void close();
    void println(std::string message, int position = -1);
    void println(Area a, std::string message, int position = -1);
    void SetArea(Area a, bool flush = false) { a_area = a; if (flush) {Flush(a);};};
    std::string ToList(Area a);
    void Flush(Area a);
    bool PrintlnBuffersEmpty();
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

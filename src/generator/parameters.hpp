#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include "../debug/debug.hpp"

#include "../ast/object_type.hpp"

#include "info_writer.hpp"

#include <string>
#include <fstream>
#include <unordered_map>

namespace generator {

  class parameters {

    Debug<false> debug;

    bool verbose = false;

    InfoWriter info_writer;
    
    /*
      <hpp file>:

      top

      class_description {

        constructor_description : constructor_initilizer_list {
	  constructor_contents
	}
	
	class_contents

	children
	
      }

      bottom

      <cpp file>:

      implementation_top

      implementation_contents

     */

    static void println(std::ostream& handle, std::string text);
    static void print(std::ostream& handle, std::string text);
    static void println(std::ostream& handle, std::list<std::string>& lines);
    static void print(std::ostream& handle, std::list<std::string>& elements, std::string delimiter = ",");
    
    struct ClassContainer {
      bool active = true;
      std::string name;
      std::string class_description;
      std::list<std::string> derived_classes;
      std::string constructor_description;
      std::list<std::string> constructor_initializer;
      std::list<std::string> constructor_contents;
      std::list<std::string> class_contents;
      std::list<ClassContainer> children;
      void flush(std::ostream& handle, int hierarchy, bool verbose);
    };
    
    struct AreaContainer {
      std::list<std::string> include;
      std::string namespace_start;
      std::list<std::string> top;
      std::list<ClassContainer> children;
      std::list<std::string> bottom;
      std::list<std::string> implementation_top;
      std::list<std::string> implementation_contents;
      std::string namespace_end;
      static const bool verbose = true;
      void flush(std::ostream& header_file, std::ostream& implementation_file, int hierarchy, bool verbose);
    };
    
    struct FileContainer {
      Debug<false> debug;
      std::string library;
      std::string file_name;
      int line_number = 1;
      AreaContainer content;
      FileContainer() : debug("FileContainer") {};
      ClassContainer* getCurrentClassContainer();
      void flush(bool verbose = false);
    };

    FileContainer file_container;

    bool quiet = false;
    
    ClassContainer* getCurrentClassContainer();
    ClassContainer* getActiveClassContainer();
    
  public:
    parameters() : debug("parameters") {};
    bool package_contains_function;
    bool parse_declarations_only = false;
    ast::ObjectValueContainer returnType;

    void addInclude(std::string text);
    void addTop(std::string text);
    void newClass(std::string description, std::string name);
    void addDerivedClass(std::string text);
    void setClassConstructorDescription(std::string text);
    void addClassConstructorInitializer(std::string text);
    void addClassConstructorContents(std::string text);
    void addClassContents(std::string text);
    void endClass();
    void addBottom(std::string text);
    void addImplementationTop(std::string text);
    void addImplementationContents(std::string text);
    
    void open(std::string filename, std::string library);
    void close();
    bool isQuiet();
    bool setQuiet(bool quiet);
    static std::string replaceFileExtension(std::string filename, std::string extension);
    static std::string baseName(std::string filename);

    void setPackageName(const std::string& path, const std::string& name);

    void printHierarchy();
  };

}

#endif

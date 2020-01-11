#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include "../debug/debug.hpp"
#include "database/global_database.hpp"
#include "database/database.hpp"
#include "../exceptions/exceptions.hpp"
#include "../ast/object_type.hpp"

#include "info_writer.hpp"

#include <string>
#include <fstream>
#include <unordered_map>

namespace generator {

  class parameters {

    Debug<false> debug;
    Exceptions exceptions;
    bool a_verbose = false;

    InfoWriter info_writer;

    GlobalDatabase* a_global_database;
    
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
      Database database;
      ast::ObjectType type;
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
      template<typename Func>
      void traverseClassContainerHierarchy(Func callback);
      ClassContainer* getCurrentClassContainer();
      ClassContainer* getParentClassContainer();
      void flush(bool verbose = false);
    };

    FileContainer file_container;

    bool quiet = false;
    
    ClassContainer* getCurrentClassContainer();
    ClassContainer* getParentClassContainer();
    ClassContainer* getActiveClassContainer();
    
  public:
    parameters(GlobalDatabase& g, std::string& library, bool verbose = false) : debug("parameters") {
      a_global_database = &g;
      file_container.library = library;
      a_verbose = verbose;
    };
    bool package_contains_function;
    bool parse_declarations_only = false;
    ast::ObjectValueContainer returnType;

    void addInclude(std::string text);
    void addTop(std::string text);
    void newClass(std::string description, std::string name, ast::ObjectType type);
    void addDerivedClass(std::string text);
    void setClassConstructorDescription(std::string text);
    void addClassConstructorInitializer(std::string text);
    void addClassConstructorContents(std::string text);
    void addClassContents(std::string text);
    void endClass();
    void addBottom(std::string text);
    void addImplementationTop(std::string text);
    void addImplementationContents(std::string text);
    
    void open(std::string filename);
    void close();
    bool isQuiet();
    bool setQuiet(bool quiet);
    static std::string replaceFileExtension(std::string filename, std::string extension);
    static std::string baseName(std::string filename);

    void setPackageName(const std::string& path, const std::string& name);

    // Database access
    template<typename Func>
    bool findOneGeneric(DatabaseResult& object, std::string& name, Func valid, std::string package = "", std::string library = "");
    template<typename Func>
    bool findOne(DatabaseResult& object, ast::SimpleIdentifier* identifier, Func valid, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& object, std::string& name, ast::ObjectType type, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& object, std::string& name, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier, ast::ObjectType type);
    bool findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier);
    void findAll(DatabaseResults& objects, const std::string& name, std::string package = "", std::string library = "");
    void addAttribute(std::string& name, ast::ObjectArguments& arguments,
                      ast::ObjectType id, ast::Attribute* attribute,
                      ast::Text* text = NULL);
    void addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                     ast::Text* text = NULL);
    void addObject(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
		     ast::ObjectArguments arguments = ast::ObjectArguments(false),
		     ast::Text* text = NULL);
    void addObject(ast::ObjectType id, std::string& name, ast::ObjectValue type = ast::ObjectValue::NONE,
		     ast::Text* text = NULL);
    bool exists(std::string& library, std::string& package);
    bool exists(std::string& library, std::string& name, ast::ObjectType type);
    bool setVisible(std::string& name, std::string package = "", std::string library = "");
    void printDatabase(std::string name = "");
    void globalizeClass();
    
    void printAllObjects(std::string name);
    std::string getName(ast::SimpleIdentifier* i, ast::ObjectType);
    std::string getName(DatabaseResult& object);
    bool getParent(ParentInfo& p);

    std::string getLibrary();
    std::string getName();
    int getHierarchyLevel();
    
    void printHierarchy();
    void printAll();
  };

  template<typename Func>
  void parameters::FileContainer::traverseClassContainerHierarchy(Func callback) {
    debug.functionStart("traverseClassContainerHierarchy");
    std::list<ClassContainer>* c = &content.children;
    int hierarchy_level = 0;
    bool done = false;
    do {
      if (c->empty()) {
	done = true;
      } else {
	ClassContainer& x = c->back();
	if (x.active) callback(x, hierarchy_level++); else done = true;
	c = &x.children;
      }
    } while (!done);
    
    debug.functionEnd("traverseClassContainerHierarchy");
  }

  template<typename Func>
  bool parameters::findOneGeneric(DatabaseResult& object, std::string& name, Func valid, std::string package, std::string library) {
    debug.functionStart("findOneGeneric");
    bool found = false;
    int hierarchy_level = 0;
    int last_hierarchy_level = 0;
    object.library = file_container.library;
    auto class_container_callback =
      [&](ClassContainer& class_container, int hierarchy) {
	last_hierarchy_level = hierarchy;
	if (class_container.database.findOne(object, name, valid, package, library)) {
	  found = true;
	  hierarchy_level = hierarchy;
	}
	object.hierarchy.push_back(class_container.name);
      };
    file_container.traverseClassContainerHierarchy(class_container_callback);
    object.local = (last_hierarchy_level == hierarchy_level);
    for (unsigned int i = hierarchy_level; i < object.hierarchy.size(); i++) {
      object.hierarchy.pop_back();
    }
    debug.functionEnd("findOneGeneric: " + std::to_string(found));
    return found;
  }

  template<typename Func>
  bool parameters::findOne(DatabaseResult& object, ast::SimpleIdentifier* identifier, Func valid, std::string package, std::string library) {
    assert(identifier);
    std::string name = identifier->toString(true);
    bool result = true;
    if (!findOneGeneric(object, name, valid, package, library)) {
      result = false;
      exceptions.printError("Could not find declaration of \"" + name + "\"", &identifier->text);
      if (a_verbose) {
	printDatabase();
      }
    }
    return result;
  }

}

#endif

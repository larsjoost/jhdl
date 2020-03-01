#ifndef GENERATOR_PARAMETERS_HPP_
#define GENERATOR_PARAMETERS_HPP_

#include "../debug/debug.hpp"
#include "database/global_database.hpp"
#include "database/database.hpp"
#include "../exceptions/exceptions.hpp"
#include "../ast/object_type.hpp"
#include "../ast/text.hpp"

#include "info_writer.hpp"

#include <string>
#include <fstream>
#include <unordered_map>

namespace generator {

  class parameters {

    Debug<true> debug;
    Exceptions exceptions;
    bool a_verbose = false;

    InfoWriter info_writer;

    static GlobalDatabase a_global_database;
    
    /*
      <hpp file>:

      top

      class_description {

        constructor_description : constructor_initilizer_list {
	  constructor_contents
	}
	
	class_contents

	children

	class_bottom
	
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
      std::string base_name;
      std::string name;
      std::string class_description;
      std::list<std::string> derived_classes;
      std::string constructor_description;
      std::list<std::string> constructor_initializer;
      std::list<std::string> constructor_contents;
      std::list<std::string> class_contents;
      std::list<std::string> class_bottom;
      std::list<std::string> class_trailer;
      std::list<ClassContainer> children;
      void flush(std::ostream& handle, int hierarchy, bool verbose);
    };
    
    struct AreaContainer {
      std::list<std::string> include;
      std::string namespace_start;
      std::list<std::string> top;
      std::list<ClassContainer> children;
      std::list<std::string> namespace_bottom;
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
      std::string getClassContainerHierarchy(std::string first_delimiter = ".", std::string delimiter = ".", bool add_type = false);
      void getHierarchy(std::list<std::string>& current_hierarchy, bool base_hierarchy);
      void flush(bool verbose = false);
    };

  public:
    FileContainer file_container;
  private:
    bool quiet = false;
    
    ClassContainer* getCurrentClassContainer();
    ClassContainer* getParentClassContainer();
    ClassContainer* getActiveClassContainer();
    
  public:
    parameters(std::string& library, bool verbose = false) : debug("parameters") {
      file_container.library = library;
      a_verbose = verbose;
    };
    bool package_contains_function;
    bool parse_declarations_only = false;
    bool process_contains_wait;
    ast::ObjectValueContainer returnType;

    void addInclude(std::string text);
    void addTop(std::string text);
    void newClass(std::string description, std::string name, ast::ObjectType type, std::string base_name);
    void addDerivedClass(std::string text);
    void setClassConstructorDescription(std::string text);
    void addClassConstructorInitializer(std::string text);
    void addClassConstructorContents(std::string text);
    void addClassContents(std::string text);
    void addClassBottom(std::string text);
    void addClassTrailer(std::string text);
    void endClass();
    void addNamespaceBottom(std::string text);
    void addBottom(std::string text);
    void addImplementationTop(std::string text);
    void addImplementationContents(std::string text, const char* file_name, int line_number);
    void addImplementationContents(std::list<std::string>& text);
    void addTextToList(std::list<std::string>& list, std::string text, const char* file_name, int line_number);
    
    void open(std::string filename);
    void close();
    bool isQuiet();
    bool setQuiet(bool quiet);
    
    void setPackageName(const std::string& path, const std::string& name);

    // Database access
  private:
    template<typename Func>
    int findBestMatch(DatabaseResults& matches, DatabaseResult& bestMatch, Func valid);
    template<typename Func>
    bool findBestMatch(DatabaseResults& global_matches, DatabaseResults& local_matches, DatabaseResult& bestMatch, Func valid);
    void findAllLocal(DatabaseResults& objects, std::string& name, std::string& package, std::string& library);
  public:
    template<typename Func>
    bool findOneBase(DatabaseResult& object, std::string& name, Func valid, std::string package = "", std::string library = "");
    template<typename Func>
    bool findOne(DatabaseResult& object, ast::SimpleIdentifier* identifier, Func valid, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& object, std::string& name, ast::ObjectType type, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& object, std::string& name, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier, ast::ObjectType type);
    bool findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier);
    void findAll(DatabaseResults& objects, std::string& name, std::string package = "", std::string library = "");
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
    bool findObject(std::shared_ptr<LocalDatabase>& object, std::string& library, std::string& name, ast::ObjectType type = ast::ObjectType::UNKNOWN);
    bool setVisible(std::string& name, std::string package = "", std::string library = "");
    void printDatabase(std::string name = "");
    void globalizeClass(ast::Text* text);
    bool isLocal(DatabaseResult& object);
    std::string hierarchyToString(std::string delimiter = ".", bool add_type = false);
    void getHierarchy(std::list<std::string>& hierarchy, bool base_hierarchy = false);
    
    void printAllObjects(std::string name);
    std::string getName(ast::SimpleIdentifier* i, ast::ObjectType);
    std::string getName(DatabaseResult& object, int hierarchy_offset = 0);
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
	if (x.active) callback(x, ++hierarchy_level); else done = true;
	c = &x.children;
      }
    } while (!done);
    debug.functionEnd("traverseClassContainerHierarchy");
  }

  template<typename Func>
  int parameters::findBestMatch(DatabaseResults& matches,
				 DatabaseResult& bestMatch,
				 Func valid) {
    debug.functionStart("findBestMatch)");
    int found = 0;
    for (auto& i : matches) {
      if (valid(i.object)) {
	if (found == 0) {
	  bestMatch = i;
	} else  {
	  if (found == 1) {
	    exceptions.printError("More than one match of " + bestMatch.toString());
	    exceptions.printError("match #1: " + bestMatch.toString(), bestMatch.object->text); 
	  }
	  exceptions.printError("match #" + std::to_string(found + 1) + ": " + i.toString(), bestMatch.object->text); 
	}
	found++;
      }
    }
    debug.functionEnd("findBestMatch: " + std::to_string(found));
    return found;
  }

  template<typename Func>
  bool parameters::findBestMatch(DatabaseResults& global_matches,
				 DatabaseResults& local_matches,
				 DatabaseResult& bestMatch,
				 Func valid) {
    debug.functionStart("findBestMatch)");
    int found = findBestMatch(global_matches, bestMatch, valid);
    bestMatch.local = false;
    if (found == 0) {
      found = findBestMatch(local_matches, bestMatch, valid);
      bestMatch.local = true;
    }
    debug.functionEnd("findBestMatch: " + std::to_string(found));
    return (found == 1);
  }

  template<typename Func>
  bool parameters::findOneBase(DatabaseResult& object, std::string& name, Func valid, std::string package, std::string library) {
    debug.functionStart("findOneBase(name = " + name + ", package = " + package + ", library = " + library + ")");
    library = (library == "WORK") ? "" : library;
    DatabaseResults global_results;
    a_global_database.findAll(global_results, name, package, library);
    if (debug.isVerbose()) {
      std::cout << "Global database:" << std::endl;
      for (auto& i : global_results) {
	std::cout << i.toString() << std::endl;
      }
    }
    DatabaseResults local_results;
    findAllLocal(local_results, name, package, library);
    if (debug.isVerbose()) {
      std::cout << "Local database:" << std::endl;
      for (auto& i : local_results) {
	std::cout << i.toString() << std::endl;
      }
    }
    bool found = findBestMatch(global_results, local_results, object, valid);
    if (!found) {
      std::cout << "Did not find any matches of " << name << std::endl;
      for (auto& i : global_results) {
	std::cout << "Found global: " << i.toString() << std::endl;
      }
      for (auto& i : local_results) {
	std::cout << "Found local: " << i.toString() << std::endl;
      }
    } else if (debug.isVerbose()) {
      std::cout << "Found: " << object.toString() << std::endl;
    }
    debug.functionEnd("findOneBase: " + std::to_string(found));
    return found;
  }

  template<typename Func>
  bool parameters::findOne(DatabaseResult& object, ast::SimpleIdentifier* identifier, Func valid, std::string package, std::string library) {
    assert(identifier);
    std::string name = identifier->toString(true);
    bool result = true;
    if (!findOneBase(object, name, valid, package, library)) {
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

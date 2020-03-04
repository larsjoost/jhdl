#include <cassert>

#include "parameters.hpp"
#include "name_converter.hpp"

namespace generator {

  GlobalDatabase parameters::a_global_database;
  
  void parameters::open(std::string filename) {
    debug.functionStart("open(filename = " + filename + ")");
    info_writer.Open(NameConverter::replaceFileExtension(filename, ".i"));
    file_container.file_name = filename;
    debug.functionEnd("open");
  }
  
  void parameters::close() {
    debug.functionStart("close");
    file_container.flush(true);
    info_writer.Close();
    debug.functionEnd("close");
  }
  
  void parameters::println(std::ostream& handle, std::string text) {
    handle << text << std::endl;
  }
  
  void parameters::print(std::ostream& handle, std::string text) {
    handle << text;
  }
  
  void parameters::println(std::ostream& handle, std::list<std::string>& lines) {
    for (auto& i : lines) {
      println(handle, i);
    }
  }
  
  void parameters::print(std::ostream& handle, std::list<std::string>& elements, std::string delimiter) {
    std::string d;
    for (auto& i : elements) {
      std::string x = d + i;
      print(handle, x);
      d = delimiter;
    }
  }

  void parameters::addInclude(std::string text) {
    file_container.content.include.push_back(text);
  }

  void parameters::addTop(std::string text) {
    file_container.content.top.push_back(text);
  }

  void parameters::newClass(std::string description, std::string name, ast::ObjectType type,
			    std::string base_name) {
    debug.functionStart("newClass(" + description + ")");
    ClassContainer a;
    a.active = true;
    a.class_description = description;
    a.base_name = base_name;
    a.name = name;
    a.type = type;
    ClassContainer* c = file_container.getCurrentClassContainer();
    if (c == NULL) {
      file_container.content.children.push_back(a);
    } else {
      c->children.push_back(a);
    }
    debug.functionEnd("newClass");
  }

  void parameters::addDerivedClass(std::string text) {
    getActiveClassContainer()->derived_classes.push_back(text);
  }
    
  void parameters::setClassConstructorDescription(std::string text) {
    getActiveClassContainer()->constructor_description = text;
  }

  void parameters::addClassConstructorInitializer(std::string text) {
    getActiveClassContainer()->constructor_initializer.push_back(text);
  }

  void parameters::addClassConstructorContents(std::string text, const char* file_name, int line_number) {
    addTextToList(getActiveClassContainer()->constructor_contents, text, file_name, line_number);
  }

  void parameters::addClassContents(std::string text, const char* file_name, int line_number) {
    addTextToList(getActiveClassContainer()->class_contents, text, file_name, line_number);
  }

  void parameters::addClassBottom(std::string text) {
    getActiveClassContainer()->class_bottom.push_back(text);
  }

  void parameters::addClassTrailer(std::string text) {
    getActiveClassContainer()->class_trailer.push_back(text);
  }

  void parameters::endClass() {
    debug.functionStart("endClass");
    getActiveClassContainer()->active = false;
    debug.functionEnd("endClass");
  }
  
  void parameters::addBottom(std::string text) {
    file_container.content.bottom.push_back(text);
  }

  void parameters::addNamespaceBottom(std::string text) {
    file_container.content.namespace_bottom.push_back(text);
  }

  void parameters::addImplementationTop(std::string text) {
    file_container.content.implementation_top.push_back(text);
  }
  
  void parameters::addImplementationContents(std::string text, const char* file_name, int line_number) {
    addTextToList(file_container.content.implementation_contents, text, file_name, line_number);
  }
  
  void parameters::addTextToList(std::list<std::string>& list, std::string text, const char* file_name, int line_number) {
    if (debug.isVerbose()) {
      text += " // " + std::string(file_name) + ":" + std::to_string(line_number);
    }
    list.push_back(text);
  }

  void parameters::addImplementationContents(std::list<std::string>& text) {
    std::list<std::string>& a = file_container.content.implementation_contents;
    a.splice(a.end(), text);
  }

  parameters::ClassContainer* parameters::getCurrentClassContainer() {
    debug.functionStart("getCurrentClassContainer");
    ClassContainer* a = file_container.getCurrentClassContainer();
    debug.functionEnd("getCurrentClassContainer");
    return a;
  }

 parameters::ClassContainer* parameters::getParentClassContainer() {
    debug.functionStart("getParentClassContainer");
    ClassContainer* a = file_container.getParentClassContainer();
    debug.functionEnd("getParentClassContainer");
    return a;
  }
  
  parameters::ClassContainer* parameters::getActiveClassContainer() {
    debug.functionStart("getActiveClassContainer");
    ClassContainer* a = getCurrentClassContainer();
    if (a == NULL) {
      printHierarchy();
      assert(false);
    }
    debug.functionEnd("getActiveClassContainer");
    return a;
  }

  bool parameters::isQuiet() {
    return quiet;
  }

  bool parameters::setQuiet(bool quiet) {
    bool q = this->quiet;
    this->quiet = quiet;
    return q;
  }

  void parameters::setPackageName(const std::string& path, const std::string& name) {
    info_writer.SetPackageName(path, name);
  }

  void parameters::printHierarchy() {
    std::cout << "<printHierarchy>" << std::endl;
    file_container.content.flush(std::cout, std::cout, 0, true);
    std::cout << "</printHierarchy>" << std::endl;
  }

  void parameters::printAll() {
    printHierarchy();
  }

  std::string parameters::getLibrary() {
    return file_container.library;
  }

  std::string parameters::getName() {
    return getActiveClassContainer()->name;
  }
  
  int parameters::getHierarchyLevel() {
    debug.functionStart("getHierarchyLevel");
    const int library_level = 1;
    int h = 0;
    auto class_container_callback =
      [&](ClassContainer& class_container, int hierarchy) {
	debug.debug("Hierarchy = " + std::to_string(hierarchy));
	h = hierarchy + library_level; 
      };
    file_container.traverseClassContainerHierarchy(class_container_callback);
    debug.functionEnd("getHierarchyLevel: " + std::to_string(h));
    return h;
  }

  std::string parameters::hierarchyToString(std::string delimiter, bool add_type) {
    return file_container.getClassContainerHierarchy(delimiter, delimiter, add_type);
  }

  void parameters::getHierarchy(std::list<std::string>& hierarchy, bool base_hierarchy) {
    file_container.getHierarchy(hierarchy, base_hierarchy);
  }
  
}

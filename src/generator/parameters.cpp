#include <cassert>

#include "parameters.hpp"

namespace generator {

  std::string parameters::replaceFileExtension(std::string filename, std::string extension) {
    return baseName(filename) + extension;
  }

  std::string parameters::baseName(std::string filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
  }
  
  void parameters::open(std::string filename, std::string library) {
    debug.functionStart("open(filename = " + filename + ")");
    info_writer.Open(replaceFileExtension(filename, ".i"));
    file_container.file_name = filename;
    file_container.library = library;
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

  void parameters::newClass(std::string description, std::string name, ast::ObjectType type) {
    debug.functionStart("newClass(" + description + ")");
    ClassContainer a;
    a.active = true;
    a.class_description = description;
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

  void parameters::addClassConstructorContents(std::string text) {
    getActiveClassContainer()->constructor_contents.push_back(text);
  }

  void parameters::addClassContents(std::string text) {
    getActiveClassContainer()->class_contents.push_back(text);
  }

  void parameters::endClass() {
    debug.functionStart("endClass");
    getActiveClassContainer()->active = false;
    debug.functionEnd("endClass");
  }
  
  void parameters::addBottom(std::string text) {
    file_container.content.bottom.push_back(text);
  }

  void parameters::addImplementationTop(std::string text) {
    file_container.content.implementation_top.push_back(text);
  }
  
  void parameters::addImplementationContents(std::string text) {
    file_container.content.implementation_contents.push_back(text);
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

  parameters::ClassContainer* parameters::FileContainer::getParentClassContainer() {
    ClassContainer* parent = NULL;
    ClassContainer* current = NULL;
    auto class_container_callback =
      [&](ClassContainer& class_container, int hierarchy) {
	parent = current;
	current = &class_container;
      };
    traverseClassContainerHierarchy(class_container_callback);
    return parent;
  }

  std::string parameters::getLibrary() {
    return file_container.library;
  }

  std::string parameters::getName() {
    return getActiveClassContainer()->name;
  }
  
  int parameters::getHierarchyLevel() {
    int h = 0;
    auto class_container_callback =
      [&](ClassContainer& class_container, int hierarchy) {
	h = hierarchy; 
      };
    file_container.traverseClassContainerHierarchy(class_container_callback);
    return h;
  }
    
}

#include <cassert>

#include "parameters.hpp"
#include "name_converter.hpp"

namespace generator {

  GlobalDatabase parameters::a_global_database;
  
  void parameters::open(std::string filename) {
    m_debug.functionStart("open(filename = " + filename + ")", false, __FILE__, __LINE__);
    info_writer.Open(NameConverter::replaceFileExtension(filename, ".i"));
    file_container.file_name = filename;
    m_debug.functionEnd("open");
  }
  
  void parameters::close() {
    m_debug.functionStart("close", false, __FILE__, __LINE__);
    file_container.flush(true);
    info_writer.Close();
    m_debug.functionEnd("close");
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
    m_debug.functionStart("newClass(" + description + ")", false, __FILE__, __LINE__);
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
    m_debug.functionEnd("newClass");
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
    m_debug.functionStart("endClass", false, __FILE__, __LINE__);
    getActiveClassContainer()->active = false;
    m_debug.functionEnd("endClass");
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
  
  void parameters::addImplementationContents(std::string text, const char* file_name, int line_number, ast::Text* source_line) {
    addTextToList(file_container.content.implementation_contents, text, file_name, line_number, false, source_line);
  }
  
  void parameters::addTextToList(std::list<std::string>& list, std::string text,
				 const char* file_name, int line_number, bool breakpoint,
				 ast::Text* source_line) {
    if (breakpoint) {
      list.push_back("std::raise(SIGINT);");
    }
    if (source_line) {
      text = "try { " + text + " } catch (const vhdl::RuntimeError& e) { SC_REPORT_FATAL(\"VHDL code:\", \"" +
	source_line->getFilename() + "(" + std::to_string(source_line->getLine()) + "): " + NameConverter::makeStringPrintable(source_line->getCurrentLine()) + "\"); };"; 
    }
    if (m_debug.isVerbose()) {
      text += " // " + std::string(file_name) + ":" + std::to_string(line_number);
    }
    list.push_back(text);
  }

  void parameters::addImplementationContents(std::list<std::string>& text) {
    std::list<std::string>& a = file_container.content.implementation_contents;
    a.splice(a.end(), text);
  }

  parameters::ClassContainer* parameters::getCurrentClassContainer() {
    m_debug.functionStart("getCurrentClassContainer", false, __FILE__, __LINE__);
    ClassContainer* a = file_container.getCurrentClassContainer();
    m_debug.functionEnd("getCurrentClassContainer");
    return a;
  }

 parameters::ClassContainer* parameters::getParentClassContainer() {
    m_debug.functionStart("getParentClassContainer", false, __FILE__, __LINE__);
    ClassContainer* a = file_container.getParentClassContainer();
    m_debug.functionEnd("getParentClassContainer");
    return a;
  }
  
  parameters::ClassContainer* parameters::getActiveClassContainer() {
    m_debug.functionStart("getActiveClassContainer", false, __FILE__, __LINE__);
    ClassContainer* a = getCurrentClassContainer();
    if (a == NULL) {
      printHierarchy();
      assert(false);
    }
    m_debug.functionEnd("getActiveClassContainer");
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
    m_debug.functionStart("getHierarchyLevel", false, __FILE__, __LINE__);
    const int library_level = 1;
    int h = 0;
    auto class_container_callback =
      [&](ClassContainer& class_container, int hierarchy) {
	m_debug.debug("Hierarchy = " + std::to_string(hierarchy));
	h = hierarchy + library_level; 
      };
    file_container.traverseClassContainerHierarchy(class_container_callback);
    m_debug.functionEnd("getHierarchyLevel: " + std::to_string(h));
    return h;
  }

  std::string parameters::hierarchyToString(std::string delimiter, bool add_type) {
    return file_container.getClassContainerHierarchy(delimiter, delimiter, add_type);
  }

  void parameters::getHierarchy(std::list<std::string>& hierarchy, bool base_hierarchy) {
    file_container.getHierarchy(hierarchy, base_hierarchy);
  }
  
}

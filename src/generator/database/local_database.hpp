#ifndef _LOCAL_DATABASE_HPP_
#define _LOCAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../../debug/debug.hpp"
#include "general.hpp"
#include "name_map.hpp"

namespace generator {

  class LocalDatabase {

    Debug<false> m_debug;

    NameMap a_content;

    bool a_verbose = false;
    
    Exceptions a_exceptions;

    void add(std::string& name, DatabaseElement& e);

    void printAllObjects(std::string& name);

  public:
    LocalDatabase() : m_debug(this) {};

    template <typename Func>
    void findAll(DatabaseResults& results, const std::string& name, Func action);
    void findAll(DatabaseResults& results, const std::string& name);

    bool setVisible(std::string name = "");

    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
             ast::ObjectArguments arguments = ast::ObjectArguments(false),
             ast::Text* text = NULL);
    void addAttribute(std::string& name, ast::ObjectArguments& arguments,
                      ast::ObjectType id, ast::Attribute* attribute,
                      ast::Text* text = NULL);
    void addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType,
                     ast::FunctionDeclaration* function,
                     ast::Text* text = NULL);

    std::string getLibrary();
    std::string getName();
    ast::ObjectType getType();
    
    void print();
  };

  template <typename Func>
  void LocalDatabase::findAll(DatabaseResults& results, const std::string& name,
			      Func action) {
    m_debug.functionStart("findAll(name = " + name + ")");
    std::list<DatabaseElement>* e = a_content.find(name);
    int result_size = 0;
    if (e) {
      for (auto& j : *e) {
	DatabaseResult r;
	r.object = &j;
	action(r);
	results.push_back(r);
	result_size++;
      }
    }
    m_debug.functionEnd("findAll: " + std::to_string(result_size));
  }


}

#endif

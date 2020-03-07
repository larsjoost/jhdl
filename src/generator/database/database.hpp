#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <unordered_map>
#include <memory>
#include <list>

#include "../../exceptions/exceptions.hpp"
#include "../../debug/debug.hpp"
#include "general.hpp"
#include "local_database.hpp"

namespace generator {

  class Database {

    Debug<false> m_debug;

  public:
    std::shared_ptr<LocalDatabase> local_database;

  private:
    Exceptions exceptions;

    bool verbose = false;
    
  public:

    Database() : m_debug(this) {
      local_database = std::make_shared<LocalDatabase>();
    };
    
    class ObjectNotFoundException {
    public:
      std::string msg;
      ast::Text* text;
      ObjectNotFoundException(std::string msg, ast::Text* text = NULL) : msg(msg), text(text) {};
      void print() {
        Exceptions exceptions;
        exceptions.printError(msg, text);
      }
    };
    
    void addAttribute(std::string& name, ast::ObjectArguments& arguments,
                      ast::ObjectType id, ast::Attribute* attribute,
                      ast::Text* text = NULL);
    void addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                     ast::Text* text = NULL);
    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
             ast::ObjectArguments arguments = ast::ObjectArguments(false),
             ast::Text* text = NULL);
    void add(ast::ObjectType id, std::string& name, ast::ObjectValue type = ast::ObjectValue::NONE,
             ast::Text* text = NULL);

    template <typename Func>
    void findAll(DatabaseResults& objects, const std::string& name, Func action);

    void print(std::string name = "");
    void PrintLocal();
    void printAllObjects(std::string name);
    void printAll();
  };

  
  template <typename Func>
  void Database::findAll(DatabaseResults& objects, const std::string& name, Func action) {
    m_debug.functionStart("findAll");
    local_database.get()->findAll(objects, name, action);
    m_debug.functionEnd("findAll");
  }
}

#endif

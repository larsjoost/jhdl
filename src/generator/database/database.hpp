#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../../exceptions/exceptions.hpp"
#include "../../debug/debug.hpp"
#include "general.hpp"
#include "local_database.hpp"
#include "global_database.hpp"

namespace generator {

  class Database {

    Debug<false> debug;

    LocalDatabase localDatabase;
    GlobalDatabase globalDatabase;
    Exceptions exceptions;

    bool verbose = false;
    
    template<typename Func>
    bool findBestMatch(DatabaseResults& matches, DatabaseResult& bestMatch, Func valid);

    void Globalize();

  public:

    Database() : debug("Database") {};
    
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

    template<typename Func>
    bool findOne(DatabaseResult& object, std::string& name, Func valid, std::string package = "", std::string library = "");
    template<typename Func>
    bool findOne(DatabaseResult& object, ast::SimpleIdentifier* identifier, Func valid, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& object, std::string& name, ast::ObjectType type, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& object, std::string& name, std::string package = "", std::string library = "");
    bool findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier, ast::ObjectType type);
    bool findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier);
    void findAll(DatabaseResults& objects, const std::string& name, std::string package = "", std::string library = "");
    ast::ObjectValueContainer getType(std::string name, std::string package, std::string library);

    bool localize(std::string& library, std::string& name, ast::ObjectType type);
    
    std::string getLibrary();
    std::string getName();
    ast::ObjectType getType();
    
    void topHierarchyStart(std::string& library, std::string& name, ast::ObjectType type);
    void topHierarchyEnd(bool globalize);
    void descendHierarchy(const std::string& name, ast::ObjectType type);
    void ascendHierarchy();
    int getHierarchyLevel();
    
    bool setVisible(std::string& name, std::string package = "", std::string library = "");

    void GetParent(ParentInfo& parent_info);

    bool exists(std::string& library, std::string& package);

    void print(std::string name = "");
    void PrintLocal();
    void printAllObjects(std::string name);
  };

  template<typename Func>
  bool Database::findBestMatch(DatabaseResults& matches,
                               DatabaseResult& bestMatch,
                               Func valid) {
    int found = 0;
    bestMatch = {false, NULL};
    for (auto& i : matches) {
      if (valid(i.object)) {
        if (bestMatch.object == NULL || (!bestMatch.local && i.local) ||
            (bestMatch.object->hierarchyLevel < i.object->hierarchyLevel)) {
          bestMatch = i;
          if (found) {
            if (found == 1) {
              exceptions.printError("More than one match of " + bestMatch.toString());
              exceptions.printError("match #1: " + bestMatch.toString()); 
            }
            exceptions.printError("match #" + std::to_string(found + 1) + ": " + i.toString()); 
          }
          found++;
        }
      }
    }
    return (found == 1);
  }

  template<typename Func>
  bool Database::findOne(DatabaseResult& object, std::string& name, Func valid, std::string package, std::string library) {
    DatabaseResults objects;
    findAll(objects, name, package, library);
    return findBestMatch(objects, object, valid);
  }
  
  template<typename Func>
  bool Database::findOne(DatabaseResult& object, ast::SimpleIdentifier* identifier, Func valid, std::string package, std::string library) {
    DatabaseResults objects;
    assert(identifier);
    std::string name = identifier->toString(true);
    bool result = true;
    if (!findOne(object, name, valid, package, library)) {
      result = false;
      exceptions.printError("Could not find declaration of \"" + name + "\"", &identifier->text);
    }
    return result;
  }
  
}

#endif

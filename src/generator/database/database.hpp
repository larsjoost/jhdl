#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../../exceptions/exceptions.hpp"
#include "general.hpp"
#include "local_database.hpp"
#include "global_database.hpp"

namespace generator {

  class Database {
    LocalDatabase localDatabase;
    GlobalDatabase globalDatabase;
    Exceptions exceptions;

    bool verbose = false;
    
    template<typename Func>
    bool findBestMatch(DatabaseResults& matches, DatabaseResult& bestMatch, Func valid);
  public:

    class ObjectNotFoundException {
    public:
      std::string msg;
      ObjectNotFoundException(std::string msg) : msg(msg) {};
    };
    
    void globalize();

    void addAttribute(std::string& name, ast::ObjectArguments& arguments,
                      ast::ObjectType id, ast::Attribute* attribute,
                      ast::Text* text = NULL);
    void addFunction(std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                     ast::Text* text = NULL);
    void addProcedure(std::string& name, ast::ObjectArguments& arguments,
                      ast::ProcedureDeclaration* procedures,
                      ast::Text* text = NULL);
    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
             ast::ObjectArguments arguments = ast::ObjectArguments(false),
             ast::Text* text = NULL);
    void add(ast::ObjectType id, std::string& name, ast::ObjectValue type = ast::NONE,
             ast::Text* text = NULL);

    template<typename Func>
    bool findOne(DatabaseResult& object, std::string& name, Func valid, std::string package = "", std::string library = "");
    template<typename Func>
    void findAll(DatabaseResults& objects, std::string& name, Func valid, std::string package = "", std::string library = "");
    ast::ObjectValueContainer getType(std::string name, std::string package, std::string library);
    
    std::string globalName(DatabaseResult& object, std::string name);
    bool globalName(std::string& name, ast::ObjectType id);

    void setLibrary(std::string& name);
    std::string getLibrary();
    void setPackage(std::string& name, bool body);
    std::string getPackage();
    void setEntity(std::string& name);
    std::string getEntity();
    void setArchitecture(std::string& name);
    std::string getSection();
    
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    bool setVisible(std::string& name, std::string package = "", std::string library = "");

    std::string getParentName(int hierarchy = 0);

    bool exists(std::string& library, std::string& package);

    void print(std::string name = "");
    void printAllObjects(std::string& name);
  };

  template<typename Func>
  bool Database::findBestMatch(DatabaseResults& matches,
                               DatabaseResult& bestMatch,
                               Func valid) {
    int found = 0;
    bestMatch = {-1, false, NULL};
    for (auto& i : matches) {
      if (valid(i.object)) {
        if (bestMatch.object == NULL || (!bestMatch.local && i.local) || (bestMatch.hierarchyLevel > i.local)) {
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
    findAll(objects, name, valid, package, library);
    return findBestMatch(objects, object, valid);
  }
  
  template<typename Func>
  void Database::findAll(DatabaseResults& objects, std::string& name, Func valid, std::string package, std::string library) {
    if ((package.empty() || package == localDatabase.getPackage()) &&
        (library.empty() || library == localDatabase.getLibrary())) {
      localDatabase.find(objects, name);
    }
    globalDatabase.find(objects, name, package, library);
  }

}

#endif

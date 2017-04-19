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
    
    template<typename Func>
    bool findBestMatch(DatabaseResults& matches, DatabaseResult& bestMatch, Func valid);
  public:

    void append(std::string& library, Database& database);

    void addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::Attribute* attribute);
    void addFunction(std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function);
    void addProcedure(std::string& name, ast::ObjectArguments& arguments, ast::ProcedureDeclaration* procedures);
    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type, ast::ObjectArguments arguments = ast::ObjectArguments(false));

    template<typename Func>
    bool findOne(DatabaseResult& object, std::string& name, Func valid, std::string package = "", std::string library = "");
    template<typename Func>
    void findAll(DatabaseResults& objects, std::string& name, Func valid, std::string package = "", std::string library = "");
    
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    bool setVisible(std::string& name, std::string package = "", std::string library = "");
 };

  template<typename Func>
  bool Database::findBestMatch(DatabaseResults& matches,
                               DatabaseResult& bestMatch,
                               Func valid) {
    bool found = false;
    bestMatch = {-1, false, NULL};
    for (auto& i : matches) {
      if (valid(i.object)) {
        if (bestMatch.object == NULL || (!bestMatch.local && i.local) || (bestMatch.hierarchyLevel > i.local)) {
          bestMatch = i;
          if (found) {
            exceptions.printError("More than one match");
          }
          found = true;
        }
      }
    }
    return found;
  }

  template<typename Func>
  bool Database::findOne(DatabaseResult& object, std::string& name, Func valid, std::string package, std::string library) {
    DatabaseResults objects;
    findAll(objects, name, valid, package, library);
    return findBestMatch(objects, object, valid);
  }
  
  template<typename Func>
  void Database::findAll(DatabaseResults& objects, std::string& name, Func valid, std::string package, std::string library) {
    if (package.empty() && library.empty()) {
      localDatabase.find(objects, name);
    }
    globalDatabase.find(objects, name, package, library);
  }

}

#endif

#ifndef GENERATOR_NAME_CONVERTER_HPP_
#define GENERATOR_NAME_CONVERTER_HPP_

#include "../debug/debug.hpp"

#include "database/database.hpp"
#include "database/general.hpp"

namespace generator {

  class NameConverter {

    Debug<false> a_debug = Debug<false>("NameConverter");

    Database *a_database;
  
  public:

    NameConverter(Database& database) {
      a_database = &database;
    };

    std::string ToLower(std::string s);
    std::string ToUpper(std::string s);
    
    std::string getPrefix(DatabaseResult& object, std::string first_separator, std::string last_separator);

    std::string getName(DatabaseResult& object, bool fullName);
    std::string getName(std::string& name, ast::ObjectArguments& arguments,
                        ast::ObjectValueContainer& return_type);
    std::string GetName(ast::SimpleIdentifier* i, ast::ObjectType);
    
  };

}

#endif

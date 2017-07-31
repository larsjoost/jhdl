#ifndef GENERATOR_NAME_CONVERTER_HPP_
#define GENERATOR_NAME_CONVERTER_HPP_

#include "../debug/debug.hpp"

#include "database/database.hpp"
#include "database/general.hpp"

namespace generator {

  class NameConverter {

    DisableDebug a_debug = DisableDebug("NameConverter");
    Database *a_database;
  
  public:

    NameConverter(Database& database) {
      a_database = &database;
    };

    std::string getPrefix(DatabaseResult& object, std::string first_separator, std::string last_separator);

    std::string getName(DatabaseResult& object, bool fullName);
    
    
  };

}

#endif

#ifndef GENERATOR_NAME_CONVERTER_HPP_
#define GENERATOR_NAME_CONVERTER_HPP_

#include "../debug/debug.hpp"
#include "../ast/object_type.hpp"

#include "parameters.hpp"

namespace generator {

  class NameConverter {

  public:

    static std::string toLower(std::string s);
    static std::string toUpper(std::string s);

    static std::string globalPrefix(parameters& parm, DatabaseResult& object, bool factory_extension = false, bool global_scope = false, int hierarchy_offset = 0);
    static std::string getPrefix(parameters& parm, DatabaseResult& object, std::string first_separator, std::string last_separator);

    static std::string getName(parameters& parm, DatabaseResult& object, bool factory_extension = false, std::string factory_arguments = "", bool global_scope = false);
    static std::string getName(std::string& name, ast::ObjectArguments& arguments,
			       ast::ObjectValueContainer& return_type);
    static std::string getHierarchyOffsetName(parameters& parm, DatabaseResult& object, int hierarchy_offset);
    
    static std::string baseName(std::string filename);

    static std::string replaceFileExtension(std::string filename, std::string extension);

    static std::string getHeaderFileName(std::string& library, std::string& file_name);
    
    static std::string objectName(ast::ObjectType type, const std::string& name);
  };

}

#endif

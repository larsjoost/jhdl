#ifndef _GENERATOR_CONFIG_HPP
#define _GENERATOR_CONFIG_HPP

#include <string>
#include <unordered_map>
#include "../exceptions/exceptions.hpp"
#include "../debug/debug.hpp"

class Config {

  Debug<false> m_debug;

  Exceptions exceptions;
  
  std::string m_filename;
  
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map;
  
public:
  Config() : m_debug(this) {};
    
  void toLower(std::string& s);
  void toUpper(std::string& s);
  void removeWhitespace(std::string& s);
  std::string expandEnvironmentVariables(const std::string& text );
  
  void add(std::string section, std::string key, std::string value);
  bool load(std::string filename);
  std::string find(std::string section, std::string key, bool expand = true);
};

#endif

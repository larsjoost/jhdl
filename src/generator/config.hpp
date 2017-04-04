#ifndef _GENERATOR_CONFIG_HPP
#define _GENERATOR_CONFIG_HPP

#include <string>
#include <unordered_map>

class Config {

  std::string filename;
  
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map;
  
public:
  void printError(std::string message);
  
  void toLower(std::string& s);
  void removeWhitespace(std::string& s);
  std::string expandEnvironmentVariables(const std::string& text );
  
  void add(std::string& section, std::string& key, std::string& value);
  void loadFile(std::string filename);
  std::string find(std::string section, std::string key, bool expand = true);
};

#endif

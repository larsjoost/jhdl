#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cstdlib>

#include "config.hpp"

void Config::toLower(std::string& s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

void Config::removeWhitespace(std::string& s) {
  s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
}

void Config::printError(std::string message) {
  std::cerr << "#Error: " << message << std::endl;
}

bool Config::load(std::string filename) {
  int lineNumber = 0;
  this->filename = filename;
  std::ifstream cFile (filename);
  if (cFile.is_open()) {
    std::string line;
    std::string currentSection = "unnamed";
    while(getline(cFile, line)) {
      lineNumber++;
      removeWhitespace(line);
      if (!line.empty()) {
        if(line[0] == '[') {
          int delimiterPos = line.find("]");
          if (delimiterPos != std::string::npos) {
            currentSection = line.substr(1, delimiterPos-1);
            toLower(currentSection);
          } else {
            printError("Missing end ] in line " + std::to_string(lineNumber) + " of file " + filename);
          }
        } else if(line[0] != '#') {
          int delimiterPos = line.find("=");
          std::string key = line.substr(0, delimiterPos);
          toLower(key);
          std::string value = line.substr(delimiterPos + 1);
          add(currentSection, key, value);
        }
      }
    }
    cFile.close();
    return true;
  }
  return false;
}

void Config::save() {
  std::ofstream cFile(filename);
  for (auto i : map) {
    cFile << "[" << i.first << "]" << std::endl;
    for (auto j : i.second) {
      cFile << j.first << " = " << j.second << std::endl;
    }
  }
  cFile.close();
}

std::string Config::expandEnvironmentVariables(const std::string& text)  {
  size_t start;
  size_t end;
  size_t size;
  size_t pos = text.find("$");
  if (pos != std::string::npos) {
    start = pos + 1;
    size = 1;
    if (text.at(start) == '{') {
      start++;
      size++;
      end = text.find("}");
    } else {
      end = text.find("/");
    }
    size_t len = (end == std::string::npos) ? std::string::npos : end - start;
    size = (len == std::string::npos) ? std::string::npos : len + size;
    std::string envName = text.substr(start, len);
    char const* e = getenv(envName.c_str());
    if (e) {
      std::string s(e);
      std::string r = text;
      r.replace(pos, size, s);
      return expandEnvironmentVariables(r);
    } else {
      printError("Environment variable " + envName + " is not defined");
    }
  }
  return text;
}

std::string Config::find(std::string section, std::string key, bool expand) {
  std::string s = section.empty() ? "unnamed" : section;
  auto x = map.find(s);
  if (x != map.end()) {
    auto y = x->second.find(key);
    if (y != x->second.end()) {
      std::string x = y->second;
      if (expand) {
        x = expandEnvironmentVariables(x);
      }
      return x;
    }
  }
  printError("Could not find key " + key + " in section " + section + " of config file " + filename);
  return "";
}

void Config::add(std::string section, std::string key, std::string value) {
  toLower(key);
  std::string s = section.empty() ? "unnamed" : section;
  auto x = map.find(s);
  if (x != map.end()) {
    x->second[key] = value;
  }
  std::unordered_map<std::string, std::string> m;
  m[key] = value;
  map[section] = m;
}
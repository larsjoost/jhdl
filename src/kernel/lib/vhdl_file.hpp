#ifndef VHDL_FILE_HPP
#define VHDL_FILE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>

#include "std_standard.hpp"

namespace vhdl {

  template<class T>
  class vhdl_file {

    std::string m_name;
    std::string m_file_name;
    int m_line_number;

    STD::STANDARD::FILE_OPEN_KIND_enum a_direction;
    std::string a_filename;
    std::ofstream* fout = NULL;
    bool file = false;
  public:

    vhdl_file(const char* name, const char* file_name, int line_number) : m_name(name), m_file_name(file_name), m_line_number(line_number){};

    vhdl_file() { }
  
    vhdl_file(STD::STANDARD::FILE_OPEN_KIND_enum direction, std::string filename) {
      a_direction = direction;
      a_filename = filename;
      fout = NULL;
      if (direction == STD::STANDARD::FILE_OPEN_KIND_enum::WRITE_MODE) {
	if (filename != "STD_OUTPUT") {
	  file = true;
	}
      }
    }

    ~vhdl_file() {
      if (file) {
	if (fout) {
	  std::cout << "Dealloc" << std::endl;
	  fout->close();
	  delete fout;
	  fout = NULL;
	}
      }
    }
  
    void construct() {
      // TODO: Implement
    }

    void write(std::string& s) {
      // std::cout << "Write = " + s << std::endl;
      if (file) {
	if (fout == NULL) {
	  // std::cout << "Alloc" << std::endl;
	  fout = new std::ofstream(a_filename);
	}
	// std::cout << "Output" << std::endl;
	*fout << s;
      } else {
	std::cout << s;
      }
    }

    void init(const vhdl_file<T>& s) {
    }
 

  };

}

#endif

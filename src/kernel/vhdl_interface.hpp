
#ifndef VHDL_INTERFACE_HPP
#define VHDL_INTERFACE_HPP

#include <cassert>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <exception>
#include <memory>

namespace vhdl {
  
  /*
    T1: sc_signal<T2>, sc_in<T2>, sc_out<T2>
    T2: 
  */

  template<class T1, class T2>
  class interface {

    std::string m_name;
    int m_line_number;
 
    T1 m_data;
    T2 m_type;
  public:

    interface<T1, T2>() {};
    
    interface<T1, T2>(const char* name) : m_data(name), m_name(name) {};

    void constrain(T2 other) {
      m_type.constrain(other);
    }
  
    T1& getInterfaceReference() {
      return m_data;
    }
    
    T1* getInterfacePointer() {
      return &m_data;
    }

    void operator () (auto& other) {
      m_data(other);
    }
    
    interface<T1, T2>& operator=(const interface<T1, T2>& s) {
      m_data = s.m_data;
      return *this;
    }

    interface<T1, T2>& operator=(auto other) {
      T2 d;
      d = other;
      m_data = d;
      return *this;
    }

    inline bool operator!=(interface<T1, T2>& other) {
      return m_data != other.m_data;
    }

    inline bool operator!=(auto other) {
      T2 x;
      x = other;
      return (m_data.read() != x);
    }

    inline bool operator==(interface<T1, T2>& other) {
      return m_data == other.m_data;
    }

    inline bool operator==(auto other) {
      return m_data == other.get();
    }

    inline bool operator!() {
      return !m_type;
    }

    inline operator bool() const {
      return bool(m_data.read());
    }
  
    template<typename TYPE>
    inline interface<T1, T2> operator+(TYPE other) {
      interface<T1, T2> r;
      r.m_data = m_data + other;
      return r;
    }

    inline unsigned int LENGTH() {
      return m_type.LENGTH();
    }
    

    inline const std::string toString() const {
      return m_data.read().toString();
    }

    inline const T2& read() const {
      return m_data.read();
    }
    
  };

  template<class T>
  class access {

    T a_value;

    bool a_null = true;
  
  public:

    T& ALL() {
      assert(!a_null);
      return a_value;
    }

    void set(T& s) {
      a_null = false;
      a_value.set(s);
    }

    void constrain(access<T> s) {
      a_value.constrain(s.a_value);
    }
  
    bool isNull() {
      return a_null;
    };

    void DEALLOCATE() {
      a_null = true;
    }
  
  };

  enum FILE_DIRECTION {READ_MODE, WRITE_MODE, UNKNOWN};

  template<class T>
  class file_access {
    FILE_DIRECTION a_direction;
    std::string a_file_name;
    std::ofstream* fout = NULL;
    bool a_file_access = false;
  public:

    file_access() { }
  
    file_access(FILE_DIRECTION direction, std::string file_accessname) {
      a_direction = direction;
      a_file_name = file_accessname;
      fout = NULL;
      if (direction == WRITE_MODE) {
	if (file_accessname != "STD_OUTPUT") {
	  a_file_access = true;
	}
      }
    }

    ~file_access() {
      if (a_file_access) {
	if (fout) {
	  std::cout << "Dealloc" << std::endl;
	  fout->close();
	  delete fout;
	  fout = NULL;
	}
      }
    }
  
    void write(std::string& s) {
      // std::cout << "Write = " + s << std::endl;
      if (a_file_access) {
	if (fout == NULL) {
	  // std::cout << "Alloc" << std::endl;
	  fout = new std::ofstream(a_file_name);
	}
	// std::cout << "Output" << std::endl;
	*fout << s;
      } else {
	std::cout << s;
      }
    }

    void init(const file_access<T>& s) {
    }
 

  };

}

#endif

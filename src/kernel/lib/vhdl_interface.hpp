
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

namespace vhdl {
  
  /*
    T1: sc_signal<T2>, sc_in<T2>, sc_out<T2>
    T2: 
  */

  template<class T1, class T2>
  class interface {
    T1 data;
    T2 type;
  public:
    std::string name;
  
    void construct(T2 other) {
      type.construct(other);
    }
  
    T1* getInterface() {
      return &data;
    }
    
    interface<T1, T2>& operator=(const interface<T1, T2>& s) {
      data = s.data;
      return *this;
    }

    interface<T1, T2>& operator=(auto other) {
      T2 d;
      d = other;
      data = d;
      return *this;
    }

    bool operator!=(interface<T1, T2>& other) {
      return data != other.data;
    }

    bool operator!=(auto other) {
      T2 x;
      x = other;
      return (data.read() != x);
    }

    bool operator==(interface<T1, T2>& other) {
      return data == other.data;
    }

    bool operator==(auto other) {
      return data == other;
    }

    interface<T1, T2> operator!() {
      interface<T1, T2> x;
      x.data = !data;
      return x;
    }

    operator bool() const {
      return bool(data);
    }
  
    interface<T1, T2> operator+(interface<T1, T2>& other) {
      interface<T1, T2> r;
      r.data = data + other.data;
      return r;
    }

    template<typename TYPE>
    interface<T1, T2> operator+(TYPE other) {
      interface<T1, T2> r;
      r.data = data + other;
      return r;
    }

    unsigned int LENGTH() {
      return type.LENGTH();
    }
    

    const std::string toString() const {
      return data.read().toString();
    }

    const T2& read() const {
      return data.read();
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

    void construct(access<T> s) {
      a_value.construct(s.a_value);
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

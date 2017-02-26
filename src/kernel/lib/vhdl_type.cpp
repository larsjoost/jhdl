#include <cstdlib>
#include <climits>
#include <string>

#include "vhdl_type.hpp"

namespace vhdl {
    
  template<class T>
  void Range<T>::operator=(T v) {
    value = v;
  }
    
  template<class T>
  std::string Range<T>::toString() {
    return std::to_string(value);
  }

  template<class T>
  T Range<T>::getValue() {
    return value;
  }
    
  template<class T>
  unsigned int Range<T>::LENGTH() {
    return 32;
  }
    

}

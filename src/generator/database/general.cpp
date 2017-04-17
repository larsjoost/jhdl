#include <iostream>

#include "general.hpp"

namespace generator {
  
  void DatabaseElement::print() {
    std::cout << "      library   = " << library << std::endl;
    std::cout << "      package   = " << package << std::endl;
    std::cout << "      name      = " << name << std::endl;
    std::cout << "      arguments = " << ast::toString(arguments) << std::endl;
    std::cout << "      visible   = " << (visible ? "true" : "false") << std::endl;
    std::cout << "      attribute = " << (attribute ? "true" : "false") << std::endl;
    std::cout << "      type      = " << ast::toString(id) << std::endl;
  }

}

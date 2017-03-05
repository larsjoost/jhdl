
#include "identifier.hpp"

namespace ast {

  int Identifier::equals(Identifier& i) {
    return text.equals(i.text);
  }

  void Identifier::print(std::ostream& output) {
    text.print(output);
  }
  
  void Identifier::debug(std::ostream& output) {
    text.debug(output);
  }
  
}

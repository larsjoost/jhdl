
#include "identifier.h"

namespace ast {

  int Identifier::equals(Identifier& i) {
    return text.equals(i.text);
  }

  void Identifier::print(FILE* output) {
    text.print(output);
  }
  
  void Identifier::debug(FILE* output) {
    text.debug(output);
  }
  
}

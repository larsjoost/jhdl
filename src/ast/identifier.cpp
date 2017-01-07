
#include "identifier.h"

namespace ast {

  int Identifier::equals(Identifier& i) {
    return text.equals(i.text);
  }

}

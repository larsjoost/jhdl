
#ifndef AST_SIMPLEINDETIFIER_H_
#define AST_SIMPLEINDETIFIER_H_

#include "text.hpp"
#include "list.hpp"

namespace ast {

  class SimpleIdentifier {
  public:
    Text text;
    
    bool equals(SimpleIdentifier* b) {
      if (b) {
	return text.equals(b->text);
      }
      return false;
    }

    std::string toString(bool setCase = false) {
      return text.toString(setCase);
    }

  };
  
}

#endif

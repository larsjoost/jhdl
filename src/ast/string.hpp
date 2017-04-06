
#ifndef AST_STRING_H_
#define AST_STRING_H_

#include "text.hpp"

namespace ast {
    
  class String {
    
  public:
    
    Text text;

    std::string toString(bool removeQuotationMarks) {
      std::string s = text.toString();
      if (removeQuotationMarks) {
        if (!s.empty()) {
          s.erase(0, 1);
        }
        if (!s.empty()) {
          s.erase(s.size() - 1, 1);
        }
      }
      return s;
    }
    
  };

}

#endif

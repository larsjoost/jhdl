

#ifndef VHDL_SCANNER_DEFINES_H_
#define VHDL_SCANNER_DEFINES_H_

namespace vhdl {
  namespace scanner {
    
    enum VhdlKeyword {
      VHDL_LIBRARY,
      VHDL_USE,
      VHDL_ARCHITECTURE,
      VHDL_OF,
      VHDL_IS,
      VHDL_BEGIN,
      VHDL_END,
      VHDL_ENTITY,
      NUMBER_OF_VHDL_KEYWORDS
    };
    
    enum VhdlStandard {
      VHDL_1987,
      VHDL_1993,
      VHDL_2002,
      VHDL_2008,
      NUMBER_OF_VHDL_STANDARDS
    };

  }
    
}


#endif
#include "standard.hpp"
namespace vhdl {
  namespace STD {
    // line 104:   function "/=" (a : integer; b : integer) return boolean is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STANDARD::BOOLEAN<> vhdl_not_equal(STANDARD::INTEGER<> A, STANDARD::INTEGER<> B);
    STANDARD::BOOLEAN<> operator !=(STANDARD::INTEGER<> A, STANDARD::INTEGER<> B){
      // Foreign function call
      return vhdl_not_equal(A, B);
    }
    // line 108:   function "&" (a : string; b : string) return string is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STANDARD::STRING<> vhdl_concat(STANDARD::STRING<> A, STANDARD::STRING<> B);
    STANDARD::STRING<> operator +(STANDARD::STRING<> A, STANDARD::STRING<> B){
      // Foreign function call
      return vhdl_concat(A, B);
    }
  }
}

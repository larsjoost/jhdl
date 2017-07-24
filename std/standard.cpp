#include "standard.hpp"
namespace vhdl {
  namespace STD {
    // line 114:   function "/=" (a : integer; b : integer) return boolean is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STD::STANDARD::BOOLEAN<> vhdl_not_equal(STD::STANDARD::INTEGER<> A, STD::STANDARD::INTEGER<> B);
    STD::STANDARD::BOOLEAN<> operator !=(STD::STANDARD::INTEGER<> A, STD::STANDARD::INTEGER<> B){
      // Foreign function call
      return vhdl_not_equal(A, B);
    }
    // line 118:   function "&" (a : string; b : string) return string is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STD::STANDARD::STRING<> vhdl_concat(STD::STANDARD::STRING<> A, STD::STANDARD::STRING<> B);
    STD::STANDARD::STRING<> operator +(STD::STANDARD::STRING<> A, STD::STANDARD::STRING<> B){
      // Foreign function call
      return vhdl_concat(A, B);
    }
    // line 122:   function "+" (a : time; b : time) return time is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STD::STANDARD::TIME<> vhdl_time_add(STD::STANDARD::TIME<> A, STD::STANDARD::TIME<> B);
    STD::STANDARD::TIME<> operator +(STD::STANDARD::TIME<> A, STD::STANDARD::TIME<> B){
      // Foreign function call
      return vhdl_time_add(A, B);
    }
  }
}

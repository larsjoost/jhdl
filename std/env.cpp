#include "env.hpp"
namespace vhdl {
  namespace STD {
    // line 14:   procedure STOP (STATUS: INTEGER) is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    void vhdl_stop(STD::STANDARD::INTEGER<> STATUS);
    void ENV::STOP(STD::STANDARD::INTEGER<> STATUS){
      // Foreign function call
      vhdl_stop(STATUS);
    }
    // line 18:   procedure STOP is
    void ENV::STOP(){
    }
    // line 23:   procedure FINISH (STATUS: INTEGER) is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    void vhdl_finish(STD::STANDARD::INTEGER<> STATUS);
    void ENV::FINISH(STD::STANDARD::INTEGER<> STATUS){
      // Foreign function call
      vhdl_finish(STATUS);
    }
    // line 27:   procedure FINISH is
    void ENV::FINISH(){
    }
    // line 32:   function RESOLUTION_LIMIT return DELAY_LENGTH is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STD::STANDARD::DELAY_LENGTH<> vhdl_resolution_limit();
    STD::STANDARD::DELAY_LENGTH<> ENV::RESOLUTION_LIMIT(){
      // Foreign function call
      return vhdl_resolution_limit();
    }
  }
}

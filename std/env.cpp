#include "env.hpp"
namespace vhdl {
  namespace STD {
    // line 14:   procedure STOP (STATUS: INTEGER) is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    void vhdl_stop(STANDARD::INTEGER<> STATUS);
    void ENV::STOP(STANDARD::INTEGER<> STATUS){
      // Foreign function call
      vhdl_stop(STATUS);
    }
    // line 18:   procedure STOP is
    void ENV::STOP(){
      STOP(0);
    }
    // line 23:   procedure FINISH (STATUS: INTEGER) is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    void vhdl_finish(STANDARD::INTEGER<> STATUS);
    void ENV::FINISH(STANDARD::INTEGER<> STATUS){
      // Foreign function call
      vhdl_finish(STATUS);
    }
    // line 27:   procedure FINISH is
    void ENV::FINISH(){
      FINISH(0);
    }
    // line 32:   function RESOLUTION_LIMIT return DELAY_LENGTH is
    /*
     * This is the definition of the foreign function set as an attribute.
     * The implementation must be defined in a .cpp file in this directory.
    */
    STANDARD::DELAY_LENGTH<> vhdl_resolution_limit();
    STANDARD::DELAY_LENGTH<> ENV::RESOLUTION_LIMIT(){
      // Foreign function call
      return vhdl_resolution_limit();
      return {0, STANDARD::NS};
    }
  }
}

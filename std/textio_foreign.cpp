#include <cassert>

#include "textio.hpp"

namespace vhdl {
  namespace STD {

    /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_readline(TEXT& F, LINE& L){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read1(LINE& L, STD::STANDARD::BIT& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read2(LINE& L, STD::STANDARD::BIT& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read3(LINE& L, STD::STANDARD::BIT_VECTOR& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read4(LINE& L, STD::STANDARD::BIT_VECTOR& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read5(LINE& L, STD::STANDARD::BOOLEAN& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read6(LINE& L, STD::STANDARD::BOOLEAN& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read7(LINE& L, STD::STANDARD::CHARACTER& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read8(LINE& L, STD::STANDARD::CHARACTER& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read9(LINE& L, STD::STANDARD::INTEGER& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read10(LINE& L, STD::STANDARD::INTEGER& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read11(LINE& L, STD::STANDARD::REAL& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read12(LINE& L, STD::STANDARD::REAL& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read13(LINE& L, STD::STANDARD::STRING& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read14(LINE& L, STD::STANDARD::STRING& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read15(LINE& L, STD::STANDARD::TIME& VALUE, STD::STANDARD::BOOLEAN& GOOD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_read16(LINE& L, STD::STANDARD::TIME& VALUE){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_writeline(TEXT& F, LINE& L){
        std::string s = STD::STANDARD::STRING::IMAGE(L.ALL()); 
        F.write(s);
        L.DEALLOCATE();
      }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write1(LINE& L, STD::STANDARD::BIT VALUE, SIDE JUSTIFIED, WIDTH FIELD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write2(LINE& L, STD::STANDARD::BIT_VECTOR VALUE, SIDE JUSTIFIED, WIDTH FIELD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write3(LINE& L, STD::STANDARD::BOOLEAN VALUE, SIDE JUSTIFIED, WIDTH FIELD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write4(LINE& L, STD::STANDARD::CHARACTER VALUE, SIDE JUSTIFIED, WIDTH FIELD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write5(LINE& L, STD::STANDARD::INTEGER VALUE, SIDE JUSTIFIED, WIDTH FIELD) {
        STD::STANDARD::INTEGER i;
        std::string x = i.IMAGE(VALUE);
        if (!L.isNull()) {
          x = STD::STANDARD::STRING::IMAGE(L.ALL()) + x;
        }
        STD::STANDARD::STRING s;
        s = x;
        // std::cout << "s = " << s.IMAGE(s) << std::endl;
        L.set(s);
      }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write6(LINE& L, STD::STANDARD::REAL VALUE, SIDE JUSTIFIED, WIDTH FIELD, STD::STANDARD::NATURAL DIGITS){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write7(LINE& L, STD::STANDARD::STRING VALUE, SIDE JUSTIFIED, WIDTH FIELD){ assert(false); }
      /*
       * This is the definition of the foreign function set as an attribute.
       * The implementation must be defined in a .cpp file in this directory.
      */
      void TEXTIO::std_textio_write8(LINE& L, STD::STANDARD::TIME VALUE, SIDE JUSTIFIED, WIDTH FIELD, STD::STANDARD::TIME UNIT){ assert(false); }

  }
}

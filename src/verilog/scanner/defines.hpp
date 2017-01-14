

#ifndef VERILOG_SCANNER_DEFINES_H_
#define VERILOG_SCANNER_DEFINES_H_

namespace verilog {
  namespace scanner {
    
    enum VerilogKeyword {
      VERILOG_MODULE,
      VERILOG_ENDMODULE,
      NUMBER_OF_VERILOG_KEYWORDS
    };
    
    enum VerilogStandard {
      VERILOG_1995,
      VERILOG_2001,
      NUMBER_OF_VERILOG_STANDARDS
    };

  }
    
}


#endif

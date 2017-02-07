#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "systemc.h"
#include "test.hpp"

int sc_main(int argc, char* argv[]) {

  bool verbose = false;
  char* vcdFilename = NULL;
  
  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "d:v")) != -1) {
    switch (c)
      {
      case 'd':
        vcdFilename = optarg;
        break;
      case 'v':
        verbose = true;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
  }

  vhdl::TEST* dut = new vhdl::TEST("DUT"); 

  if (vcdFilename) {
  
    sc_trace_file* fp = sc_create_vcd_trace_file(vcdFilename);

    sc_trace(fp, dut->A, "A");

    for (int i=0; i<100; i++) {
      sc_start(1);
    }

    sc_close_vcd_trace_file(fp);
  }
    
}

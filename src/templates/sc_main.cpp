/*
  Copy from $JHDL/src/templates/sc_main.cpp
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc.h>
#include "%include%"

int sc_main(int argc, char* argv[]) {

  bool verbose = false;
  char* vcdFilename = NULL;
  
  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "o:d:v")) != -1) {
    switch (c)
      {
      case 'o':
        vcdFilename = optarg;
        break;
      case 'd':
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

  auto* dut = new WORK::Architecture_TEST_RTL("DUT"); 

  if (vcdFilename) {

    if (verbose) {std::cout << "Creating VCD file " << vcdFilename << std::endl;}
    sc_trace_file* fp = sc_create_vcd_trace_file(vcdFilename);

    // sc_trace(fp, dut->A, "A");

    /*
    for (int i=0; i<100; i++) {
      sc_start(1);
    }

    sc_close_vcd_trace_file(fp);
    */
  }

  sc_start(1, SC_US);

  SC_REPORT_FATAL("Termination", "Test not teminated with finish(0)");
  
  return 0;
}

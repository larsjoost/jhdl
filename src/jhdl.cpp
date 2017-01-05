//============================================================================
// Name        : jhdl.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vhdl/scanner.h"
#include "vhdl/DesignUnit.h"

int
main (int argc, char **argv)
{
  char *filename = NULL;
  int c;

  opterr = 0;
  while ((c = getopt (argc, argv, "f:")) != -1)
    switch (c)
      {
      case 'f':
       filename = optarg;
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

  vhdl::Scanner s;
  s.loadFile(filename);

  vhdl::DesignUnit u = vhdl::DesignUnit(s);
  u.parse();

  return 0;
}


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "parser/design_file.hpp"
#include "ast/scanner.hpp"

void usage() {
  printf("jhdl -f <filename> [-v]\n");
  printf("  -v : Verbose\n");
}

int
main (int argc, char **argv)
{
  char *filename = NULL;
  int c;
  int verbose = 0;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "f:v")) != -1) {
    switch (c)
      {
      case 'f':
        filename = optarg;
        break;
      case 'v':
        verbose = 1;
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

  try {
    parser::DesignFile designFile = parser::DesignFile(filename, verbose);
    return 0;
  } catch (const ast::SyntaxError &e) {
    if (verbose) {
      std::cerr << "Syntax error" << std::endl;
    }
  } catch (const std::exception &e) {
    if (verbose) {
      std::cerr << e.what();
    }
  }

  return 1;
}

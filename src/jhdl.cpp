
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vhdl/scanner/scanner.hpp"
#include "vhdl/parser/design_file.hpp"
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
  
  if (filename == NULL) {
    fprintf(stderr, "File name is not specified\n");
    usage();
    exit(1);
  } else {
    
    try {
      vhdl::scanner::Scanner scanner = vhdl::scanner::Scanner(verbose);
      scanner.loadFile(filename);
      scanner.accept<vhdl::parser::DesignFile>();
    } catch (ast::FileNotFound e) {
      printf("File %s not found...\n", filename);
    } catch (ast::SyntaxError e) {
      return 1;
    }
  }
  
  return 0;
}

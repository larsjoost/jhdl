
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "parser/design_file.hpp"
#include "generator/file_info.hpp"
#include "ast/scanner.hpp"
#include "version/version.h"

void usage() {
  version::Version v;
  printf("hdlf -f <filename> -e <expression> [-v]\n");
  std::cout << "Version " << v.version << " (" << v.date << "), id " << v.id << std::endl;
  printf("  -v : Verbose\n");
}

int
main (int argc, char **argv)
{
  std::string filename = "";
  char *expression = NULL;
  int c;
  bool verbose = false;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "f:ve:")) != -1) {
    switch (c)
      {
      case 'f':
        filename = optarg;
        break;
      case 'e':
        expression = optarg;
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

  try {
    parser::DesignFile parserDesignFile;
    parserDesignFile.parse(filename);
    generator::FileInfo fileInfo(parserDesignFile);
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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "parser/design_file.hpp"
#include "generator/systemc.hpp"
#include "ast/scanner.hpp"

void usage() {
  printf("hdlc -f <name> [-l <name>] [-s <name>] [-v]\n");
  printf("  -f <name> : File name\n");
  printf("  -l <name> : Library name\n");
  printf("  -c <name> : Configuration file name\n");
  printf("  -s        : Save library info file\n"); 
  printf("  -v : Verbose\n");
}

int
main (int argc, char **argv)
{
  std::string filename = "";
  int c;
  bool verbose = false;
  std::string library = "work";
  std::string configurationFilename = "";
  bool saveLibraryInfo = false;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "f:l:c:sv")) != -1) {
    switch (c)
      {
      case 'f':
        filename = optarg;
        break;
      case 'l':
        library = optarg;
        break;
      case 'c':
        configurationFilename = optarg;
        break;
      case 's':
        saveLibraryInfo = true;
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
        usage();
        return 1;
      default:
        abort ();
      }
  }
  
  try {
    auto parserDesignFile = parser::DesignFile();
    parserDesignFile.parse(filename);
    auto systemC = generator::SystemC(verbose);
    systemC.generate(parserDesignFile, library, configurationFilename);
    if (saveLibraryInfo) {
      systemC.saveLibraryInfo();
    }
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

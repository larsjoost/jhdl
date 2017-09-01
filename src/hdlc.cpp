
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "parser/design_file.hpp"
#include "generator/systemc.hpp"
#include "ast/scanner.hpp"
#include "version/version.h"

void usage() {
  version::Version v;
  std::cout << "hdlc -f <name> [-l <name>] [-s <name>] [-v]" << std::endl;
  std::cout << "Version " << v.version << " (" << v.date << "), id " << v.id << std::endl;
  std::cout << "  -f <name> : File name" << std::endl;
  std::cout << "  -l <name> : Library name (default = work)"  << std::endl;
  std::cout << "  -c <name> : Configuration file name" << std::endl;
  std::cout << "  -s        : Save library info file"  << std::endl;
  std::cout << "  -v : Verbose" << std::endl;
}

int
main (int argc, char **argv)
{
  std::string filename = "";
  int c;
  bool verbose = false;
  std::string library = "WORK";
  std::string configurationFilename = "";
  bool saveLibraryInfo = false;
  bool standardPackage = false;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "f:l:c:svp")) != -1) {
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
      case 'p':
        standardPackage = true;
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
    parser::DesignFile parserDesignFile;
    parserDesignFile.parse(filename);
    generator::SystemC systemC(verbose);
    systemC.generate(parserDesignFile, library, configurationFilename, standardPackage);
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

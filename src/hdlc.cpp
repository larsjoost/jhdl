
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "parser/design_file.hpp"
#include "generator/systemc.hpp"
#include "generator/name_converter.hpp"
#include "generator/file_info.hpp"
#include "ast/scanner.hpp"
#include "version/version.h"
#include "debug/debug.hpp"
#include "exceptions/exceptions.hpp"

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
  Debug<false> debug = Debug<false>("main");
  std::string filename = "";
  int c;
  bool verbose = false;
  std::string library = "WORK";
  std::string configurationFilename = "";
  bool save_info = false;
  bool standardPackage = false;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "f:l:c:svp")) != -1) {
    switch (c)
      {
      case 'f':
        filename = optarg;
        break;
      case 'l':
        library = generator::NameConverter::toUpper(optarg);
        break;
      case 'c':
        configurationFilename = optarg;
        break;
      case 's':
        save_info = true;
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
    debug.debug("Parsing " + filename);
    parserDesignFile.parse(filename);
    generator::SystemC systemC(verbose);
    debug.debug("Generating SystemC files");
    systemC.generate(parserDesignFile, library, configurationFilename, standardPackage);
    if (save_info) {
      systemC.saveLibraryInfo();
      generator::FileInfo file_info(filename);
      file_info.write_dependencies(parserDesignFile, ".d");
      file_info.write_makefile_dependencies(parserDesignFile, ".mak");
    }
    Exceptions exceptions;
    return exceptions.errorsExists();
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

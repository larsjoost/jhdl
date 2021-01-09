
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "parser/design_file.hpp"
#include "generator/systemc.hpp"
#include "generator/file_info.hpp"
#include "generator/name_converter.hpp"
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
  std::cout << "  -o <path> : Output path (default = ./)"  << std::endl;
  std::cout << "  -c <name> : Configuration file name" << std::endl;
  std::cout << "  -e <text> : Output file info in <text> format"  << std::endl;
  generator::FileInfo::help(4);
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
  bool standardPackage = false;
  std::string output_expression;
  std:string output_path;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "f:l:c:e:vo:p")) != -1) {
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
      case 'e':
        output_expression = optarg;
        break;
      case 'o':
        output_path = optarg;
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
    generator::SystemC systemC(output_expression, verbose);
    debug.debug("Generating SystemC files");
    systemC.generate(parserDesignFile, library, configurationFilename, standardPackage, output_path);
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

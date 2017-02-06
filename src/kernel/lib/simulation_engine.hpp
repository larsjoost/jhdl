#ifndef _KERNEL_SIMULATION_ENGINE_HPP_
#define _KERNEL_SIMULATION_ENGINE_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

template<class T>
class SimulationEngine {

  T* simulationUnit = NULL;

  const char* doFilename = NULL;
  
  bool verbose = false;

  void usage() {
    std::cout << "-h        : Help" << std::endl;
    std::cout << "-d <name> : Do filename" << std::endl;
  }
  
public:
  
  SimulationEngine(int argc, char* argv[]) {
    opterr = 0;
    char c;
    while ((c = getopt (argc, argv, "d:vh")) != -1) {
      switch (c)
	{
	case 'd':
	  doFilename = optarg;
	  break;
	case 'v':
	  verbose = true;
	  break;
	case 'h':
	  usage();
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
	  exit(1);
	default:
	  abort ();
	}
    }
  }

  std::vector<std::string> split(std::string& s, char delimiter) {
    std::vector<std::string> a;
    std::stringstream ss(s);
    std::string tok;
    while(getline(ss, tok, delimiter)) {
      a.push_back(tok);
    }
    return a;
  }
  
  void parseCommand(std::string& s) {
    std::vector<std::string> a = split(s, ' ');
    if (a[0] == "help") {
      std::cout << "help   : This menu" << std::endl;
      std::cout << "quit   : Quit simulation" << std::endl;
      std::cout << "run    : run for one time step" << std::endl;
    } else if (a[0] == "run") {
      int number = 1;
      if (a.size() > 1) {
        std::istringstream(a[1]) >> number;
      }
      if (!simulationUnit) {
        simulationUnit = new T();
      }
      std::cout << "run for " << number << std::endl;
      for (int i=0; i<number; i++) {
        simulationUnit->start();
      }
    } else if (a[0] == "verbose") {
      simulationUnit->verbose = true;
    }
    
  }
  
  void run() {

    std::string command;

    if (doFilename) {
      std::ifstream infile(doFilename);
      while (std::getline(infile, command)) {
        std::cout << "> " << command << std::endl;
	parseCommand(command);
      }
    }
    
    while (command != "quit") {
      std::cout << "> ";
      getline(std::cin, command);
      parseCommand(command);
    }

  }
};

#endif

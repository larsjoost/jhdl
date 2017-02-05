#ifndef _KERNEL_SIMULATION_ENGINE_HPP_
#define _KERNEL_SIMULATION_ENGINE_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

template<class T>
class SimulationEngine {

    T* simulationUnit = NULL;

public:
  SimulationEngine(int argc, char* argv[]) {

  }

  std::vector<std::string> split(std::string s, char delimiter) {
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
      std::cout << "Starting" << std::endl;
      for (int i=0; i<number; i++) {
        simulationUnit->start();
      }
    } else if (a[0] == "verbose") {
      simulationUnit->verbose = true;
    }
    
  }
  
  void run() {

    std::string command;

    while (command != "quit") {
      std::cout << "> ";
      std::cin >> command;
      parseCommand(command);
    }

  }
};

#endif

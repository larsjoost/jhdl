#ifndef _KERNEL_SIMULATION_ENGINE_HPP_
#define _KERNEL_SIMULATION_ENGINE_HPP_

#include <iostream>
#include <string>

template<class T>
class SimulationEngine {

public:
  SimulationEngine(int argc, char* argv[]) {

  }

  void run() {
    T* a = NULL;

    std::string command;

    while (command != "quit") {
      std::cout << "> ";
      std::cin >> command;
      if (command == "run") {
        if (!a) {
          a = new T();
        }
        std::cout << "Starting" << std::endl;
        a->start();
      } else if (command == "verbose") {
        a->setVerbose(true);
      }
    }

  }
};

#endif

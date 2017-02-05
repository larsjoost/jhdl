#include "simulation_engine.hpp"
#include "%include%"

int main(int argc, char* argv[]) {

  auto e = SimulationEngine<vhdl::%module%>(argc, argv);

  e.run();

}

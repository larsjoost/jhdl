#include "simulation_engine.hpp"
#include "test.hpp"

int main(int argc, char* argv[]) {

  auto e = SimulationEngine<vhdl::TEST>(argc, argv);

  e.run();

}

#include <iostream>

extern int sc_main(int argc, char* argv[]);
extern int run(int argc, char* argv[]);

int main(int argc, char* argv[]) {

  sc_main(argc, argv);

  std::cout << "Here" << std::endl;
  
  run(argc, argv);

}

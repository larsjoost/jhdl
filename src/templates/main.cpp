#include <iostream>

extern int sc_main(int argc, char* argv[]);
extern int run(int argc, char* argv[]);

int main(int argc, char* argv[]) {

  sc_main(argc, argv);

  int exitCode = run(argc, argv);

  return exitCode;
}

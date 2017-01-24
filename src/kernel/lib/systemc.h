
#ifndef SYSTEMC_H
#define SYSTEMC_H

#include <cstdlib>

#define SC_MODULE(x) class x 

#define SC_CTOR(x)  x()

#define SC_METHOD(x) x()

#define sc_main main

#define finish(x) exit(x)

#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <exception>

#include "omp.h"

#include "systemc.h"

sc_now_t sc_now = {0, SC_NS};

int exitCode;

std::vector<sc_thread*> methods;
std::vector<sc_signal_base*> signals;

int deltaCycle = 0;
bool methodEvent = false;

bool verbose = false;

std::string toBinary(int x, int binaryWidth) {
  std::string s = "";
  for (int i=0; i<binaryWidth; i++) {
    char c = ((x % 2) == 0 ? '0' : '1');
    s = c + s;
    x >>= 1;
  }
  return s;
}

void log(sc_trace_file* handle, int value, unsigned int dataWidth, int traceId) {
  if (handle) {
    *handle << "#" << sc_now.value << std::endl;
    *handle << "b" << toBinary(value, dataWidth) << " " << std::to_string(traceId) << std::endl;
  }
}

void sc_module::AddMethod(sc_thread* c) {
  methods.push_back(c);
}

 
sc_trace_file* sc_create_vcd_trace_file(const char* name) {
  sc_trace_file* out = new sc_trace_file(name);
  *out << "$timescale 1ns $end" << std::endl;
  *out << "$scope module top $end" << std::endl;
  return out;
};

void sc_close_vcd_trace_file(sc_trace_file* fp) {
  
}

int traceId = 1;

template<class T>
void sc_trace(sc_trace_file* fh, sc_signal<T>& s, const char* name) {
  int dataWidth = s.LENGTH();
  *fh << "$var wire " << dataWidth << " " << traceId << " " << name << " $end" << std::endl;
  *fh << "$upscope $end" << std::endl;
  *fh << "$enddefinitions $end" << std::endl;
  *fh << "$dumpvars" << std::endl;
  s.traceId = traceId++;
  s.name = name;
  s.fileHandle = fh;
}

void sc_start(int runs) {
  if (verbose) {std::cout << "[MAIN] start" << std::endl;}
  deltaCycle = 0;
  for (int i=0; i<runs; i++) {
    if (verbose) {std::cout << "[MAIN] method event = " << methodEvent << std::endl;}
    deltaCycle = 0;
    if (verbose) {std::cout << "[MAIN] delta cycle = " << deltaCycle << ", sc_now = " << sc_now.toString() << std::endl;}
    do {
      methodEvent = false;
      unsigned int i;
      unsigned int size;
      size = methods.size();
#pragma omp parallel for        
      for (i = 0; i < size; i++) {
        methods[i]->process();
      }
      size = signals.size();
#pragma omp parallel for        
      for (i = 0; i < size; i++) {
        signals[i]->latchValue();
      }
      deltaCycle++;
    } while (methodEvent);
    sc_now.inc();
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
  
bool parseCommand(std::string& s) {
  if (s.size() > 0) {
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
      sc_start(number);
    } else if (a[0] == "verbose") {
      verbose = true;
    }
  }
  return true;
}
  
void usage() {
  std::cout << "-h        : Help" << std::endl;
  std::cout << "-d <name> : Do filename" << std::endl;
}

int run(int argc, char* argv[]) {
  const char* doFilename = NULL;

  opterr = 0;
  optind = 1;
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
  
  std::string command;

  bool continueRun = true;
  
  if (doFilename) {
    if (verbose) {std::cout << "Loading do file " << doFilename << "..." << std::endl;}
    std::ifstream infile(doFilename);
    while (std::getline(infile, command)) {
      std::cout << "> " << command << std::endl;
      continueRun = parseCommand(command);
      if (!continueRun) {
        break;
      }
    }
  } else {
    std::cout << "No do file specified" << std::endl;
  }
  
  while (command != "quit" && continueRun) {
    std::cout << "> ";
    getline(std::cin, command);
    continueRun = parseCommand(command);
  }
  
  while (!methods.empty()) {
    sc_thread* t = methods.back();
    delete t;
    methods.pop_back();
  }
  
  return exitCode;
  
}


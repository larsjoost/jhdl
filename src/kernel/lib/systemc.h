
#ifndef SYSTEMC_H
#define SYSTEMC_H

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

//extern vhdl::STANDARD::NOW;

enum SC_UNITS {SC_FS, SC_PS, SC_NS, SC_US, SC_MS, SC_SEC, SC_MIN, SC_HR};

int sc_now = 0;

bool runThreads = false;
bool terminateThreads = false;

std::mutex mMutex;

std::condition_variable masterWait;
std::condition_variable methodWait;

std::vector<std::thread*> methods;

std::atomic<int> numberOfMethods;
int numberOfMethodsDone = 0;
int deltaCycle = 0;
bool methodEvent = false;

std::mutex logMutex;

typedef std::ofstream sc_trace_file;

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

void log(sc_trace_file* handle, int value, int traceId) {
  if (handle) {
    logMutex.lock();
    *handle << "#" << sc_now << std::endl;
    *handle << "b" << toBinary(value, 32) << " " << std::to_string(traceId) << std::endl;
    logMutex.unlock();
  }
}

void sc_exit(int i) {
  std::lock_guard<std::mutex> lk(mMutex);
  terminateThreads = true;
  masterWait.notify_all();
  methodWait.notify_all();
}

class TerminateThreads : std::exception {};

template<class T>
class sc_thread {

  void (T::*threadFunction)() = NULL;

  T* classContainer = NULL;
  
 public:
  sc_thread(void (T::*f)(), T* c) {
    threadFunction = f;
    classContainer = c;
  }

  void run() {
    if (verbose) {std::cout << "[METHOD] Waiting on start" << std::endl;}
    std::unique_lock<std::mutex> lk(mMutex);
    if (!runThreads) {
      methodWait.wait(lk, []() {return runThreads;});
    }
    lk.unlock();
    if (verbose) {std::cout << "[METHOD] Starting" << std::endl;}
    try {
      (*classContainer.*threadFunction)();
    } catch (TerminateThreads e) {
      if (verbose) {std::cout << "Catched TerminateThreads" << std::endl;}
    }
  }
  
};

class sc_module {
  
 public:
  const char* name = NULL;

  sc_module(const char* name) : name(name) {};
  
  template<class T>
  void addMethod(auto f, T* c) {
    auto* t = new sc_thread<T>(f, c);
    std::thread* th = new std::thread(&sc_thread<T>::run, t);
    methods.push_back(th);
    numberOfMethods++;
  };
  
  void wait(int i) {
    // Wait until main() sends data
    if (terminateThreads) {
      throw TerminateThreads();
    }
    std::unique_lock<std::mutex> lk(mMutex);
    int n = sc_now + i;
    methodEvent = true;
    do {
      if (terminateThreads) {
        lk.unlock();
        throw TerminateThreads();
      }
      int currentDeltaCycle = deltaCycle;
      numberOfMethodsDone++;
      masterWait.notify_all();
      if (verbose) {std::cout << "[METHOD " << i << "] waiting on delta cycle " << deltaCycle << std::endl;}
      methodWait.wait(lk, [&]() {return (currentDeltaCycle != deltaCycle) || terminateThreads;});
    } while (sc_now < n);
    lk.unlock();
    if (verbose) {std::cout << "[METHOD " << i << "] wait done!!!!!!!!" << std::endl;}
  }
  
};

template<class T>
class sc_signal {
 public:
  std::string name;
  sc_trace_file* fileHandle = NULL;
  int traceId;
  T signal;

  sc_signal<T>() { }

  sc_signal<T>(const sc_signal<T>& s) {
    signal = s.signal;
    log(fileHandle, signal.getValue(), traceId);
  }

  sc_signal<T>& operator=(const sc_signal<T>& s) {
    signal = s.signal;
    log(fileHandle, signal.getValue(), traceId);
    return *this;
  }

  sc_signal<T> operator=(auto v) {
    signal = v;
    log(fileHandle, signal.getValue(), traceId);
    return *this;
  }

  sc_signal<T> operator+(auto v) {
    signal = signal + v;
    return *this;
  }

};

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
  *fh << "$var wire 32 " << traceId << " " << name << " $end" << std::endl;
  *fh << "$upscope $end" << std::endl;
  *fh << "$enddefinitions $end" << std::endl;
  *fh << "$dumpvars" << std::endl;
  s.traceId = traceId++;
  s.name = name;
  s.fileHandle = fh;
}

bool sc_start(int runs) {
  if (verbose) {std::cout << "[MAIN] start" << std::endl;}
  std::unique_lock<std::mutex> lk(mMutex);
  if (verbose) {std::cout << "[MAIN] lock" << std::endl;}
  deltaCycle = 0;
  for (int i=0; i<runs; i++) {
    if (verbose) {std::cout << "[MAIN] method event = " << methodEvent << std::endl;}
    if (methodEvent || (deltaCycle == 0)) {
      deltaCycle++;
    } else {
      deltaCycle = 0;
      sc_now++;
    }
    if (verbose) {std::cout << "[MAIN] delta cycle = " << deltaCycle << ", sc_now = " << sc_now << std::endl;}
    methodEvent = false;
    numberOfMethodsDone = 0;
    runThreads = true;
    //      vhdl::STANDARD::NOW.value = sc_now;
    if (terminateThreads) {
      lk.unlock();
      return false;
    }
    if (verbose) {std::cout << "[MAIN] wait" << std::endl;}
    methodWait.notify_all();
    masterWait.wait(lk, []() {return (numberOfMethodsDone == numberOfMethods) || terminateThreads;});
    if (verbose) {std::cout << "[MAIN] woke up" << std::endl;}
  }
  return !terminateThreads;
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
    bool continueRun = sc_start(number);
    if (!continueRun) {
      int i = 0;
      for (std::thread* th : methods) {
        if (verbose) {std::cout << "Waiting for thread " << i++ << " to exit" << std::endl;}
        th->join();
      }
      return false;
    }
  } else if (a[0] == "verbose") {
    verbose = true;
  }
  return true;
}
  
void usage() {
  std::cout << "-h        : Help" << std::endl;
  std::cout << "-d <name> : Do filename" << std::endl;
}

void run(int argc, char* argv[]) {
  const char* doFilename = NULL;
  const char* vcdFilename = NULL;

  opterr = 0;
  optind = 1;
  char c;
  while ((c = getopt (argc, argv, "d:o:vh")) != -1) {
    switch (c)
      {
      case 'd':
        doFilename = optarg;
        break;
      case 'o':
        vcdFilename = optarg;
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
  
}

#define SC_CTOR(x) x(const char* name) : sc_module(name)

#define SC_MODULE(x) class x : public sc_module

#define SC_METHOD(x) addMethod(x, this)

#endif

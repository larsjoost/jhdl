
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

//extern vhdl::STANDARD::NOW;

enum SC_UNITS {SC_FS, SC_PS, SC_NS, SC_US, SC_MS, SC_SEC, SC_MIN, SC_HR};

int sc_now = 0;

std::mutex m;
std::condition_variable masterWait;
std::condition_variable methodWait;

std::vector<std::thread*> methods;

std::atomic<int> numberOfMethods;

std::atomic<int> numberOfMethodsDone;

std::mutex logMutex;

typedef std::ofstream sc_trace_file;

bool verbose = false;

void log(sc_trace_file* handle, const std::string& msg) {
  std::cout << "#Handle = " << handle << std::endl;
  if (handle) {
    std::cout << "#Out" << std::endl;
    logMutex.lock();
    std::cout << "#Lock" << std::endl;
    *handle << msg << std::endl;
    *handle << "#" << sc_now << std::endl;
    logMutex.unlock();
    std::cout << "#Unlock" << std::endl; 
  }
}

class sc_module {
  
 public:
  bool verbose = false;
  const char* name;

  sc_module(const char* name) : name(name) {};
  
  template<class T>
  void addMethod(auto f, T* c) {
    std::thread* th = new std::thread(f, c);
    methods.push_back(th);
    numberOfMethods++;
  };
  
  void wait(int i) {
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    int n = sc_now + i;
    do {
      if (verbose) {std::cout << "Waiting until now (" << sc_now << ") = " << n << std::endl;}
      numberOfMethodsDone++;
      masterWait.notify_all();
      methodWait.wait(lk);
    } while(sc_now < n);
    if (verbose) {std::cout << "Wait done at now = " << sc_now << std::endl;}
    lk.unlock();
  }
  
};

template<class T>
class sc_signal {
  sc_trace_file* fileHandle = NULL;
public:
  std::string name;
  T signal;

  sc_signal<T>() { }

  sc_signal<T>(const sc_signal<T>& s) {
    std::cout << "File handle = " << fileHandle << std::endl;
    signal = s.signal;
    log(fileHandle, signal.toString() + name);
  }

  sc_signal<T>& operator=(const sc_signal<T>& s) {
    std::cout << "File handle = " << fileHandle << std::endl;
    signal = s.signal;
    log(fileHandle, signal.toString() + name);
    return *this;
  }

  void setFileHandle(sc_trace_file* fp) {
    std::cout << "Setting file handle to " << fp << std::endl;
    fileHandle = fp;
  }
  
  sc_signal<T> operator=(auto v) {
    signal = v;
    log(fileHandle, signal.toString() + name);
    return *this;
  }

  sc_signal<T> operator+(auto v) {
    signal = signal + v;
    return *this;
  }

};

sc_trace_file* sc_create_vcd_trace_file(const char* name) {
  sc_trace_file* out = new sc_trace_file();
  out->open(name, std::ios::out);
  if (!out->is_open()) {
    std::cerr << "Error: Cannot open file " << *name << std::endl;
    exit(1);
  }
  return out;
};

void sc_close_vcd_trace_file(sc_trace_file* fp) {
  
}

template<class T>
void sc_trace(sc_trace_file* f, sc_signal<T>& s, const char* name) {
  *f << "$var wire 32 " << name << " " << name << "$end" << std::endl;
  *f << "$enddefinitions $end" << std::endl;
  *f << "$dumpvars" << std::endl;
  s.name = name;
  s.setFileHandle(f);
}

void sc_start(int runs) {
  for (int i=0; i<runs; i++) {
    {
      if (verbose) {std::cout << "Starting" << std::endl;}
      std::unique_lock<std::mutex> lk(m);
      numberOfMethodsDone = 0;
      if (verbose) {std::cout << "main() signals data ready for processing\n";}
      sc_now++;
      //      vhdl::STANDARD::NOW.value = sc_now;
      methodWait.notify_all();
    }
  
    {
      if (verbose) {std::cout << "Main lock" << std::endl;}
      std::unique_lock<std::mutex> lk(m);
      while (numberOfMethodsDone != numberOfMethods) {
        if (verbose) {std::cout << "Main sleep" << std::endl;}
        masterWait.wait(lk);
        if (verbose) {std::cout << "Main woke up" << std::endl;}
      }
      if (verbose) {std::cout << "All methods are done" << std::endl;}
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
    sc_start(number);
  } else if (a[0] == "verbose") {
    verbose = true;
  }
  
}
  
void usage() {
  std::cout << "-h        : Help" << std::endl;
  std::cout << "-d <name> : Do filename" << std::endl;
}

void run(int argc, char* argv[]) {
  const char* doFilename = NULL;
  const char* vcdFilename = NULL;
  
  opterr = 0;
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

#define SC_CTOR(x) x(const char* name) : sc_module(name)

#define SC_MODULE(x) class x : public sc_module

#define SC_METHOD(x) addMethod(x, this)

#endif

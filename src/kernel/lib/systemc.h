
#ifndef SYSTEMC_H
#define SYSTEMC_H

#include <cassert>
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

enum SC_UNITS {SC_FS, SC_PS, SC_NS, SC_US, SC_MS, SC_SEC, SC_MIN, SC_HR};

extern int sc_now;

extern bool runThreads;
extern bool terminateThreads;
extern int exitCode;

extern std::mutex mMutex;

extern std::condition_variable masterWait;
extern std::condition_variable methodWait;

class sc_thread;

class sc_signal_base {
public:
  virtual void latchValue() = 0;
  virtual std::string toString() = 0; 
};

extern std::vector<std::thread*> threads;
extern std::vector<sc_thread*> methods;
extern std::vector<sc_signal_base*> signals;

extern std::atomic<int> numberOfMethods;
extern int numberOfMethodsDone;
extern int deltaCycle;
extern bool methodEvent;

extern std::mutex logMutex;

typedef std::ofstream sc_trace_file;

extern bool verbose;

std::string toBinary(int x, int binaryWidth);

void log(sc_trace_file* handle, int value, unsigned int dataWidth, int traceId);

class TerminateThreads : std::exception {};

void sc_exit(int i);

class sc_thread {

 public:

  virtual void process() = 0;
  
  void threadLoop();

  template<typename Func>
  void wait(Func waitUntil) {
    // Wait until main() sends data
    if (terminateThreads) {
      throw TerminateThreads();
    }
    std::unique_lock<std::mutex> lk(mMutex);
    methodEvent = true;
    do {
      if (terminateThreads) {
        lk.unlock();
        throw TerminateThreads();
      }
      int currentDeltaCycle = deltaCycle;
      numberOfMethodsDone++;
      masterWait.notify_all();
      if (verbose) {std::cout << "[METHOD] waiting on delta cycle " << deltaCycle << std::endl;}
      methodWait.wait(lk, [&]() {return (currentDeltaCycle != deltaCycle) || terminateThreads;});
    } while (!waitUntil());
    lk.unlock();
    if (verbose) {std::cout << "[METHOD] wait done!!!!!!!!" << std::endl;}
  }
  
  void wait(int i);
  void wait();

};

class sc_module {
  
 public:
  const char* name = NULL;

  sc_module(const char* name) : name(name) {};
  sc_module() {};
  
  void addMethod(sc_thread* c);  
  
};

template<class T>
class sc_signal : public sc_signal_base {
 public:
  std::string name;
  sc_trace_file* fileHandle = NULL;
  int traceId;
  T currentValue;
  T nextValue;
  bool event = false;
  
  explicit sc_signal<T>() {
      signals.push_back(this);
  }

  void latchValue() {
    event = (currentValue == nextValue) ? false : true;
    // std::cerr << "LatchValue of " << name << ", current " << currentValue.toString() << ", next = " << nextValue.toString() << ", event = " << event << std::endl;
    currentValue = nextValue;
    if (event) {
      log(fileHandle, currentValue.getValue(), currentValue.LENGTH(), traceId);
    }
  }
  
  sc_signal<T>(const sc_signal<T>& s) {
      nextValue = s.currentValue;
  }

  sc_signal<T>& operator=(const sc_signal<T>& s) {
    nextValue = s.currentValue;
    return *this;
  }

  sc_signal<T> operator=(auto other) {
    nextValue = other;
    return *this;
  }

  bool operator!=(sc_signal<T>& other) {
    return currentValue != other.currentValue;
  }

  bool operator!=(auto other) {
    return currentValue != other;
  }

  bool operator==(sc_signal<T>& other) {
    return currentValue == other.currentValue;
  }

  bool operator==(auto other) {
    return currentValue == other;
  }

  sc_signal<T> operator!() {
    sc_signal<T> x;
    x.nextValue = !currentValue;
    return x;
  }

  operator bool() const {
    return bool(currentValue);
  }
  
  T operator+(sc_signal<T>& other) {
    T r;
    r = currentValue + other.currentValue;
    return r;
  }

  unsigned int LENGTH() {
    return currentValue.LENGTH();
  }

  bool EVENT() {
    return event;
  }

  std::string toString() {
    return currentValue.toString();
  }

  std::string STATUS() {
    return "Current = " + currentValue.STATUS() +
      ", Next = " + nextValue.STATUS() +
      ", event = " + std::to_string(event);
  }
  
};

template<class T>
class sc_in {

  sc_signal<T>* s = NULL;

 public:
  
  void bind(sc_signal<T>& other) {
    s = &other;
  }
  
  bool EVENT() {
    if (s) {
      return s->EVENT();
    }
    return false;
  }

  T operator+(sc_in<T>& v) {
    T r = *s + *(v.s);
    return r;
  }

  std::string STATUS() {
    if (s) {
      return s->STATUS();
    }
    return "s is 0";
  }
  

};

template<class T>
class sc_out {

  sc_signal<T>* s = NULL;

  sc_out<T>(const sc_out<T>& other) {}

public:
  
  sc_out<T>() {}


  void bind(sc_signal<T>& other) {
    s = &other;
  }
  
  sc_out<T>& operator=(const sc_out<T>& other) {
    *s = *other.s;
    return this;
  }

  sc_signal<T> operator=(const sc_signal<T>& other) {
    assert(s);
    *s = other;
    return *s;
  }

  sc_signal<T> operator=(const T other) {
    assert(s);
    *s = other;
    return *s;
  }

  std::string STATUS() {
    if (s) {
      return s->STATUS();
    }
    return "s is 0";
  }
  
};

sc_trace_file* sc_create_vcd_trace_file(const char* name);

void sc_close_vcd_trace_file(sc_trace_file* fp);

extern int traceId;

template<class T>
void sc_trace(sc_trace_file* fh, sc_signal<T>& s, const char* name);

bool sc_start(int runs);

std::vector<std::string> split(std::string& s, char delimiter);
  
bool parseCommand(std::string& s);
  
void usage();

int run(int argc, char* argv[]);

#define SC_CTOR(x) x(const char* name) : sc_module(name)

#define SC_INTERFACE(x) struct interface_##x
#define SC_MODULE(x) struct x : public sc_module, interface_##x

#define SC_FOR_GENERATE(x) struct x  : public sc_module
#define SC_NEW_FOR_GENERATE(x) x

#define SC_PACKAGE(x) struct x 
#define SC_PACKAGE_BODY(x) struct body_##x : public x 

#define SC_BLOCK(x) struct x : public sc_module

#define SC_NEW_BLOCK(x) x

#define SC_THREAD(x) struct x : public sc_thread
#define SC_NEW_THREAD(x) addMethod(x)

#endif

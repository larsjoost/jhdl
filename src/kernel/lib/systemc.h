
#ifndef SYSTEMC_H
#define SYSTEMC_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

enum SC_UNITS {SC_FS, SC_PS, SC_NS, SC_US, SC_MS, SC_SEC, SC_MIN, SC_HR};

static int sc_now = 0;

class sc_module {
   
  std::mutex m;
  std::condition_variable cv;
 
  std::vector<std::thread*> methods;

  int numberOfMethods = 0;

  std::atomic<int> numberOfMethodsDone;
  
 public:
  bool verbose = true;

  template<class T>
  void add(auto f, T* c) {
    std::thread* th = new std::thread(f, c);
    methods.push_back(th);
    numberOfMethods++;
  };
  
  void start() {
    {
      if (verbose) {std::cout << "Starting" << std::endl;}
      std::unique_lock<std::mutex> lk(m);
      numberOfMethodsDone = 0;
      if (verbose) {std::cout << "main() signals data ready for processing\n";}
      sc_now++;
      cv.notify_all();
    }

    {
      if (verbose) {std::cout << "Main lock" << std::endl;}
      std::unique_lock<std::mutex> lk(m);
      while (numberOfMethodsDone != numberOfMethods) {
        if (verbose) {std::cout << "Main sleep" << std::endl;}
        cv.wait(lk);
        if (verbose) {std::cout << "Main woke up" << std::endl;}
      }
      if (verbose) {std::cout << "All methods are done" << std::endl;}
    }
  }

  void wait(int i) {
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    int n = sc_now + i;
    do {
      if (verbose) {std::cout << "Waiting until now (" << sc_now << ") = " << n << std::endl;}
      numberOfMethodsDone++;
      cv.notify_all();
      cv.wait(lk);
    } while(sc_now < n);
    if (verbose) {std::cout << "Wait done at now = " << sc_now << std::endl;}
    lk.unlock();
  }
  
};

#define SC_MODULE(x) class x : public sc_module

#define SC_METHOD(x) add(x, this)

#endif

#ifndef VHDL_TYPE_HPP
#define VHDL_TYPE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

namespace vhdl {
  
  template<class T>
  class Range {
  public:
    T left;
    T right;
    T value;

    class iterator {
      friend class Range;
      Range<T>* parent;
      T index;
    public:
      Range<T> operator *() const { parent->value = index; return *parent; }
      const iterator &operator ++() { ++index; return *this; }
      Range<T> operator ++(T) { iterator copy(*this); ++index; return copy; }
      
      bool operator ==(const iterator &other) const { return index == other.index; }
      bool operator !=(const iterator &other) const { return index != other.index; }

    protected:
      iterator(Range<T>* parent, T start) : parent(parent), index(start) { }
    };
    
  public:
      
    Range(T left, T right) :
      value(left), left(left), right(right),
      begin_(this, left), end_(this, right + 1) { }
    
    void operator=(T v);
    
    std::string toString();

    T getValue();
    
    static ::std::string IMAGE(Range<T>& r) {
      return ::std::to_string(r.value);
    }

    unsigned int LENGTH();
    
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    iterator begin_;
    iterator end_;

  };

  template <class T, typename Units>
  class Physical {
  protected:
  public:
    T value;
    Units units;

    Physical() {};
    
    Physical(T value, Units units) :
      value(value), units(units) { }

    static ::std::string IMAGE(Physical<T, Units>& r) {
      return std::to_string(r.value) + " " + toString(r.units);
    }

  };

  template <typename T>
  struct Enumeration {
  public:
    T value;

    T getValue();
    
    void operator=(T v);
    
    static ::std::string IMAGE(Enumeration<T>& r) {
      return std::to_string(r.value);
    }
  
  };
  
}

#endif

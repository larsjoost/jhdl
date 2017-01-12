
#ifndef AST_LIST_H_
#define AST_LIST_H_

#include <list>

namespace ast {

  template<class T> class List {
  public:
    std::list<T> list;
    void add(T* t);
  };

  template<class T>
  void List<T>::add(T* t) {
    if (t) {
      list.push_back(*t);
    }
  };

}

#endif

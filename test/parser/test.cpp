#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>

int
main (int argc, char **argv)
{
  class Child {
  public:
    int a;
    void c() {
    }
  };

  class Parent : public Child {
  public:
    int b;
  };

  
  Parent a;
  Parent b;
  
  std::list<Child> l;

  a.b = 2;
  a.a = 1;
  a.c();
  
  l.push_back(a);
  
  a.b = 3;
  a.a = 5;
  
  for (std::list<Child>::iterator it = l.begin(); it != l.end(); it++) {
    Child d = *it;
    printf("d.a = %u\n", d.a);
  }

}

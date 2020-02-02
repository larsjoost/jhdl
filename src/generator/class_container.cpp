#include <cassert>

#include "parameters.hpp"

namespace generator {

  void parameters::ClassContainer::flush(std::ostream& handle, int hierarchy, bool verbose) {
    if (verbose) {
      handle << "// Area Content (name = " + name + ", hierarchy = " + std::to_string(hierarchy) +
	", active = " + std::string(active ? "true" : "false") + ")" << std::endl;
    }
    /*
      class_description {

        constructor_description : constructor_initilizer_list {
	  constructor_contents
	}
	
	class_contents

	children
	
      }
    */
    assert(!class_description.empty());
    print(handle, class_description);
    if (!derived_classes.empty()) {
      print(handle, " : public ");
      print(handle, derived_classes);
    }
    println(handle, " {");
    if (!constructor_description.empty()) {
      print(handle, constructor_description);
      if (!constructor_initializer.empty()) {
	print(handle, " : ");
	print(handle, constructor_initializer, ",");
      }
      println(handle, " {");
      println(handle, constructor_contents);
      println(handle, "}; // constructor " + name + " end");
    } else {
      assert(constructor_initializer.empty());
      assert(constructor_contents.empty());
    }
    println(handle, class_contents);
    for (auto& i : children) {
      i.flush(handle, hierarchy + 1, verbose);
    }
    println(handle, class_bottom);
    println(handle, "}; // class " + name + " end");
    println(handle, class_trailer);
  }
  
}

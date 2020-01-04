#include <cassert>

#include "parameters.hpp"

namespace generator {

  void parameters::AreaContainer::flush(std::ostream& header_file, std::ostream& implementation_file, int hierarchy, bool verbose) {
    println(header_file, include);
    println(header_file, namespace_start);
    println(header_file, top);
    for (auto& i : children) {
      i.flush(header_file, hierarchy, verbose);
    }
    println(header_file, bottom);
    println(header_file, namespace_end);
    println(implementation_file, implementation_top);
    println(implementation_file, namespace_start);
    println(implementation_file, implementation_contents);
    println(implementation_file, namespace_end);
  }

}

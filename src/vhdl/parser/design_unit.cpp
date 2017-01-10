
#include "design_unit.hpp"
#include "architecture.hpp"
#include "entity.hpp"
#include "context_clause.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {

    void DesignUnit::parse(scanner::Scanner* scanner) {
      scanner->optional<ContextClause>();
      scanner->optional<Entity>();
      scanner->optional<Architecture>();
    }
    
  }
}

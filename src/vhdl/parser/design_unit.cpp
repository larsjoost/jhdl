
#include "../../ast/scanner.hpp"
#include "design_unit.hpp"
#include "architecture.hpp"
#include "entity.hpp"
#include "context_clause.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {

    void DesignUnit::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      module.contextClause = scanner->optional<ContextClause>();
      module.interface = scanner->optional<Entity>();
      module.implementation = scanner->optional<Architecture>();
    }
    
  }
}

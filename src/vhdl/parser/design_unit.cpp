
#include "../../ast/scanner.hpp"
#include "design_unit.hpp"
#include "architecture.hpp"
#include "entity.hpp"
#include "package.hpp"
#include "context_clause.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {

    void DesignUnit::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      m_debug.functionStart("parse");
      ((module.contextClause = scanner->optional<ContextClause>()) ||
       (module.interface = scanner->optional<Entity>()) ||
       (module.implementation = scanner->optional<Architecture>()) ||
       (module.package = scanner->optional<Package>()));
      m_debug.functionEnd("parse");
    }
    
  }
}

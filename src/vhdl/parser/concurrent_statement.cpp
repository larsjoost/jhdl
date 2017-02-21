

#include "../../ast/scanner.hpp"
#include "../../ast/concurrent_statement.hpp"
#include "../scanner/scanner.hpp"
#include "concurrent_statement.hpp"
#include "process.hpp"
#include "forgenerate_statement.hpp"
#include "block_statement.hpp"

namespace vhdl {
  namespace parser {
  
    ConcurrentStatement* ConcurrentStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (method = scanner->optional<Process>()) ||
	(forGenerateStatement = scanner->optional<ForGenerateStatement>()) ||
        (blockStatement = scanner->optional<BlockStatement>());
      return this;
    }
    
  }
}


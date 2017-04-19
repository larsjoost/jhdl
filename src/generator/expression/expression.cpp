#include "expression.hpp"

namespace generator {


  ast::ObjectValueContainer ExpressionParser::getAttributeType(ast::ObjectValueContainer& type,
                                                               std::string attributeName) {
    static std::unordered_map<std::string, ast::ObjectValueContainer> t =
      {{"IMAGE", ast::ObjectValueContainer(ast::USER_TYPE, "STRING")},
       {"LENGTH", ast::ObjectValueContainer(ast::INTEGER)}};
    auto i = t.find(attributeName);
    if (i != t.end()) {
      return i->second;
    } else {
      exceptions.printError("Could not resolve type of attribute " + attributeName);
      assert(false);
    }
  }
  
  void ExpressionParser::findAttributeMatch(DatabaseResults& objects,
                                            DatabaseResult& match,
                                            ast::ObjectValueContainer& expectedType,
                                            std::string& name) {
    match.object = NULL;
    for (auto& i : objects) {
      ast::ObjectValueContainer a = getAttributeType(i.object->type, name);
      if (expectedType.equals(a)) {
        if (!match.object) {
          match = i;
        } else {
          exceptions.printError("Found more than one attribute match");
        }
      }
    }
  }
  
      

}

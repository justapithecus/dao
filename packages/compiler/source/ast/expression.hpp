#pragma once

#include "ast.hpp"

namespace dao {

  struct identifier_expr {
    std::string name;
  };

  struct numeral_expr {
    std::string val;
  };

  struct binary_expr {
    ast_node lhs, rhs;
    char     op;
  };

} // namespace dao

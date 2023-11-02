#pragma once

#include "ast.hpp"

namespace dao {

  struct if_expr {
    ast_node cond_, then_, else_;
  };

} // namespace dao

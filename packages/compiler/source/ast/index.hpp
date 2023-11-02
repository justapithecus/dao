#pragma once

#include "expression.hpp"
#include "external_linkage.hpp"

namespace dao {
  static_assert(sizeof(ast_node) <= 24);
}

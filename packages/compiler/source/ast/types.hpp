#pragma once

#include <ankerl/unordered_dense.h>

#include "expression.hpp"

namespace dao {

  inline auto constexpr compiler_directive_deduced_type{
    "[[dao::deduced_type]]"};

  enum class builtin_type_kind {
    e_int8,
  };

  extern ankerl::unordered_dense::map<std::string, builtin_type_kind> const
    builtin_types;

  struct type_alias {
    identifier_expr from;
    identifier_expr to;
  };
} // namespace dao

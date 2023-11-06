#pragma once

#include <ankerl/unordered_dense.h>

namespace dao {

  enum class builtin_type_kind {
    e_int8,
  };

  extern ankerl::unordered_dense::map<std::string, builtin_type_kind> const
    builtin_types;

  struct type_alias {
    std::string typename_;
  };
} // namespace dao

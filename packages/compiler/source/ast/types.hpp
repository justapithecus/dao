#pragma once

#include <ankerl/unordered_dense.h>

#include "expression.hpp"

namespace dao {

  inline auto constexpr compiler_directive_deduced_type{
    "[[dao::deduced_type]]"};

  enum class builtin_type_kind {
    e_int8,
    e_int32,
  };

  enum class type_declarator {
    e_value,
    e_pointer,
  };

  enum class type_qualifier {
    e_mutable,
    e_readonly,
  };

  extern ankerl::unordered_dense::map<std::string, builtin_type_kind> const
    builtin_types;

  struct type_alias {
    identifier_expr from;
    identifier_expr to;
  };

  struct type_expression {
    std::string     typename_;
    type_declarator declarator{type_declarator::e_value};
    type_qualifier  qualifier{type_qualifier::e_mutable};
  };
} // namespace dao

#pragma once

#include <ankerl/unordered_dense.h>
#include <optional>

#include "expression.hpp"

namespace dao {

  inline auto constexpr compiler_directive_deduced_type{
    "[[dao::deduced_type]]"};

  enum class builtin_type_kind : std::uint8_t {
    e_int8,
    e_int32,
  };

  enum class type_qualifier : std::uint8_t {
    e_mutable,
    e_readonly,
  };

  enum class type_declarator : std::uint8_t {
    e_value,
    e_pointer,
    e_reference,
  };

  extern ankerl::unordered_dense::map<std::string, builtin_type_kind> const
    builtin_types;

  struct type_alias {
    identifier_expr from;
    identifier_expr to;
  };

  struct type_expression {
    std::optional<std::string> typename_{std::nullopt};
    type_qualifier             qualifier{type_qualifier::e_mutable};
    type_declarator            declarator{type_declarator::e_value};
  };
} // namespace dao

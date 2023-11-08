#pragma once
#ifdef CTEST

#include <ankerl/unordered_dense.h>

#include "../ast/types.hpp"

namespace dao {
  inline std::array<std::string, 2> const qualifier_to_str{
    "mutable", "readonly"};

  inline ankerl::unordered_dense::map<std::string, type_qualifier> const
    str_to_qualifier{
      {"mutable", type_qualifier::e_mutable},
      {"readonly", type_qualifier::e_readonly},
    };

  inline std::array<std::string, 3> const declarator_to_str{
    "value", "pointer", "reference"};

  inline ankerl::unordered_dense::map<std::string, type_declarator> const
    str_to_declarator{
      {"value", type_declarator::e_value},
      {"pointer", type_declarator::e_pointer},
      {"reference", type_declarator::e_reference},
    };

} // namespace dao

#endif

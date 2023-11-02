#pragma once

#include "function.hpp"

namespace dao {

  enum class linkage_kind {
    e_c_linkage,
  };

  struct external_linkage_ast {
    linkage_kind   kind;
    function_proto proto;
  };
} // namespace dao

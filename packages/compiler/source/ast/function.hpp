#pragma once

#include "types.hpp"

namespace dao {

  struct function_arg {
    std::string     name;
    type_expression type_expr;
  };

  struct function_proto {
    std::string               id;
    std::vector<function_arg> args;
    type_expression           ret;
  };

  struct function_def {
    ast_node       body;
    function_proto proto;
  };

  struct function_call {
    std::string           callee;
    std::vector<ast_node> args;
  };

} // namespace dao

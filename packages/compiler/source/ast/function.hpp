#pragma once

#include <optional>

#include "ast.hpp"

namespace dao {

  struct function_arg {
    std::string                name;
    std::optional<std::string> typename_;
  };

  struct function_proto {
    std::string                id;
    std::vector<function_arg>  args;
    std::optional<std::string> ret;
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

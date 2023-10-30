#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace dao {

  struct identifier_expr;
  struct numeral_expr;
  struct binary_expr;
  struct function_arg;
  struct function_proto;
  struct function_def;
  struct function_call;

  using ast      = std::variant<numeral_expr, identifier_expr, binary_expr,
    function_proto, function_def, function_call>;
  using ast_node = std::unique_ptr<ast>;

  struct identifier_expr {
    std::string name;
  };

  struct numeral_expr {
    int val;
  };

  struct binary_expr {
    ast_node lhs, rhs;
    char     op;
  };

  struct function_arg {
    std::string name;
  };

  struct function_proto {
    std::string               id;
    std::vector<function_arg> args;
  };

  struct function_def {
    ast_node       body;
    function_proto proto;
  };

  struct function_call {
    std::string           callee;
    std::vector<ast_node> args;
  };

  static_assert(sizeof(ast_node) <= 24);

} // namespace dao

#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace dao {

  struct identifier_expr;
  struct numeral_expr;
  struct binary_expr;
  struct call_expr;
  struct prototype;
  struct function_def;

  using ast      = std::variant<numeral_expr, identifier_expr, binary_expr>;
  using ast_node = std::unique_ptr<ast>;

  // using ast_node = std::variant<number_expr, variable_expr, binary_expr, call_expr,
  //   prototype, function_def>;

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

  // struct call_expr {
  //   std::string                            callee;
  //   std::vector<std::unique_ptr<ast_node>> args;
  // };

  // struct prototype {
  //   std::string              name;
  //   std::vector<std::string> args;
  // };

  // struct function_def {
  //   std::unique_ptr<prototype> proto;
  //   std::unique_ptr<ast_node>  body;
  // };

  static_assert(sizeof(ast_node) <= 24);

} // namespace dao

#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace dao {

  struct number_expr;
  struct identifier_expr;
  struct binary_expr;
  struct call_expr;
  struct prototype;
  struct function_def;

  using ast_node = std::variant<number_expr, identifier_expr>;

  // using ast_node = std::variant<number_expr, variable_expr, binary_expr, call_expr,
  //   prototype, function_def>;

  struct number_expr {
    int val;
  };

  struct identifier_expr {
    std::string name;
  };

  // struct binary_expr {
  //   char                      op;
  //   std::unique_ptr<ast_node> lhs;
  //   std::unique_ptr<ast_node> rhs;
  // };

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

  static_assert(sizeof(ast_node) <= 56);

} // namespace dao

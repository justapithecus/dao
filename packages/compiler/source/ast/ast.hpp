#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace dao {

  struct program_ast;
  struct identifier_expr;
  struct numeral_expr;
  struct string_literal;
  struct binary_expr;
  struct function_arg;
  struct function_proto;
  struct function_def;
  struct function_call;
  struct if_expr;
  struct external_linkage_ast;
  struct type_alias;

  using ast      = std::variant<program_ast, numeral_expr, string_literal,
    identifier_expr, binary_expr, function_proto, function_def, function_call,
    if_expr, external_linkage_ast, type_alias>;
  using ast_node = std::unique_ptr<ast>;

  struct program_ast {
    /// meta
    ///
    /// Compile-time meta-programming expressions
    std::vector<ast_node> meta;

    /// entry
    ///
    /// Auto-generated program entrypoint in the absence of an explicitly
    /// user-defined main function
    std::vector<ast_node> entry;

    /// nodes
    ///
    /// AST nodes with run-time impact
    std::vector<ast_node> nodes;
  };

} // namespace dao

#pragma once
#ifdef CTEST

#include "json.hpp"

namespace dao {

  struct ast_pretty_printer {

    auto operator()(dao::identifier_expr const &expr) const -> json;
    auto operator()(dao::numeral_expr const &expr) const -> json;
    auto operator()(dao::binary_expr const &expr) const -> json;
    auto operator()(dao::function_proto const &expr) const -> json;
    auto operator()(dao::function_def const &expr) const -> json;
  };

  inline auto to_json(json &j, dao::ast const &node) -> void {
    j = std::visit(ast_pretty_printer{}, node);
  }

  inline auto from_json(json const &j, dao::ast &node) {
  }

} // namespace dao

#endif

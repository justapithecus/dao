#pragma once
#ifdef CTEST

#include "../parser/ast.hpp"
#include "json.hpp"

namespace dao {

  struct ast_pretty_printer {

    auto operator()(dao::identifier_expr const &expr) const -> json;
    auto operator()(dao::numeral_expr const &expr) const -> json;
    auto operator()(dao::binary_expr const &expr) const -> json;
    auto operator()(dao::function_proto const &expr) const -> json;
    auto operator()(dao::function_def const &expr) const -> json;
    auto operator()(dao::function_call const &expr) const -> json;
  };

  inline auto to_json(json &j, dao::function_arg const &arg) {
    j = json{{"name", arg.name}};
  }

  inline auto from_json(json const &j, dao::function_arg &arg) {
    j.at("name").get_to(arg.name);
  }

  inline auto to_json(json &j, dao::function_proto const &proto) {
    j = json{{"id", proto.id}, {"args", proto.args}};
  }

  inline auto from_json(json const &j, dao::function_proto &proto) {
    j.at("id").get_to(proto.id);
    j.at("args").get_to(proto.args);
  }

  inline auto to_json(json &j, dao::ast const &node) -> void {
    j = std::visit(ast_pretty_printer{}, node);
  }

  inline auto from_json(json const &j, dao::ast &node) {
  }

} // namespace dao

#endif

#pragma once
#ifdef CTEST

#include "../ast/index.hpp"
#include "json.hpp"
#include "types.hpp"

namespace dao {

  struct ast_pretty_printer {
    auto operator()(dao::program_ast const &) const -> json;
    auto operator()(dao::external_linkage_ast const &) const -> json;
    auto operator()(dao::identifier_expr const &) const -> json;
    auto operator()(dao::numeral_expr const &) const -> json;
    auto operator()(dao::string_literal const &) const -> json;
    auto operator()(dao::binary_expr const &) const -> json;
    auto operator()(dao::function_proto const &) const -> json;
    auto operator()(dao::function_def const &) const -> json;
    auto operator()(dao::function_call const &) const -> json;
    auto operator()(dao::if_expr const &) const -> json;
    auto operator()(dao::type_alias const &) const -> json;
  };

  inline auto to_json(json &j, dao::type_expression const &expr) {
    j = json{
      {"typename", expr.typename_.value_or(compiler_directive_deduced_type)},
      {"qualifier", qualifier_to_str[static_cast<int>(expr.qualifier)]},
      {"declarator", declarator_to_str[static_cast<int>(expr.declarator)]},
    };
  }

  inline auto from_json(json const &j, dao::type_expression &expr) {
    j.at("typename").get_to(expr.typename_);

    std::string qualifier, declarator{};
    j.at("qualifier").get_to(qualifier);
    j.at("declarator").get_to(declarator);

    if (expr.typename_ == compiler_directive_deduced_type) {
      expr.typename_ = std::nullopt;
    }

    expr.qualifier  = str_to_qualifier.at(qualifier);
    expr.declarator = str_to_declarator.at(declarator);
  }

  inline auto to_json(json &j, dao::function_arg const &arg) {
    j = json{
      {"name", arg.name},
      {"type", arg.type_expr},
    };
  }

  inline auto from_json(json const &j, dao::function_arg &arg) {
    j.at("name").get_to(arg.name);
    j.at("type").get_to(arg.type_expr);
  }

  inline auto to_json(json &j, dao::function_proto const &proto) {
    j = json{
      {"id", proto.id},
      {"args", proto.args},
      {"return_type", proto.ret},
    };
  }

  inline auto from_json(json const &j, dao::function_proto &proto) {
    j.at("id").get_to(proto.id);
    j.at("args").get_to(proto.args);
    j.at("return_type").get_to(proto.ret);
  }

  inline auto to_json(json &j, dao::ast const &node) -> void {
    j = std::visit(ast_pretty_printer{}, node);
  }

  inline auto from_json(
    [[maybe_unused]] json const &j, [[maybe_unused]] dao::ast &node) {
    // TODO(andrew): visit for loading from json
  }

} // namespace dao

#endif

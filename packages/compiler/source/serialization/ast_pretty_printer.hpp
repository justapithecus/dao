#pragma once
#ifdef CTEST

#include "../ast/index.hpp"
#include "json.hpp"

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

  auto constexpr compiler_directive_deduced_type{"[[dao::deduced_type]]"};

  inline auto to_json(json &j, dao::function_arg const &arg) {
    j = json{
      {"name", arg.name},
      {"typename", arg.typename_.value_or(compiler_directive_deduced_type)},
    };
  }

  inline auto from_json(json const &j, dao::function_arg &arg) {
    j.at("name").get_to(arg.name);
    j.at("typename").get_to(arg.typename_);

    if (arg.typename_ == compiler_directive_deduced_type) {
      arg.typename_ = std::nullopt;
    }
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

  inline auto from_json(
    [[maybe_unused]] json const &j, [[maybe_unused]] dao::ast &node) {
    // TODO(andrew): visit for loading from json
  }

} // namespace dao

#endif

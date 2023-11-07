#pragma once

#include <ankerl/unordered_dense.h>

#include "../ast/index.hpp"

namespace dao {

  struct analysis_tables {
    ankerl::unordered_dense::map<std::string, std::string> const &types_;
  };

  class semantic_analyzer {
    ankerl::unordered_dense::map<std::string, std::string> types_;

  public:
    explicit semantic_analyzer();

    auto analyze(dao::ast const &ast) -> analysis_tables;

    // Visitors
    auto operator()(dao::program_ast const &) -> void;
    auto operator()(dao::external_linkage_ast const &) -> void;
    auto operator()(dao::identifier_expr const &) -> void;
    auto operator()(dao::numeral_expr const &) -> void;
    auto operator()(dao::string_literal const &) -> void;
    auto operator()(dao::binary_expr const &) -> void;
    auto operator()(dao::function_proto const &) -> void;
    auto operator()(dao::function_def const &) -> void;
    auto operator()(dao::function_call const &) -> void;
    auto operator()(dao::if_expr const &) -> void;
    auto operator()(dao::type_alias const &) -> void;
  };
} // namespace dao

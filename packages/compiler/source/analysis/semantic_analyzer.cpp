#include "semantic_analyzer.hpp"

namespace dao {

  semantic_analyzer::semantic_analyzer() {
  }

  auto semantic_analyzer::analyze(dao::ast const &ast) -> analysis_tables {
    std::visit(*this, ast);
    // just return single table for now
    return analysis_tables{types_};
  }

  auto semantic_analyzer::operator()(dao::program_ast const &prog) -> void {
    // evaluate metaprogramming (eventually this may not always be at the beginning
    // and may be interleaved)
    std::for_each(prog.meta.begin(), prog.meta.end(),
      [this](auto &&node) { std::visit(*this, *node); });
  }

  auto semantic_analyzer::operator()(dao::external_linkage_ast const &)
    -> void {
  }

  auto semantic_analyzer::operator()(dao::identifier_expr const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::numeral_expr const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::string_literal const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::binary_expr const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::function_proto const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::function_def const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::function_call const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::if_expr const &) -> void {
  }

  auto semantic_analyzer::operator()(dao::type_alias const &meta) -> void {
    if (not types_.contains(meta.from.name) and
        not builtin_types.contains(meta.from.name)) {

      // TODO(andrew): errors - aliasing from a type that has not been defined
    }

    types_.emplace(meta.to.name, meta.from.name);
  }
} // namespace dao

#include "semantic_analyzer.hpp"

namespace dao {

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
  auto semantic_analyzer::operator()(dao::type_alias const &) -> void {
  }
} // namespace dao

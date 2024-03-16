#pragma once

#include <ankerl/unordered_dense.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include "../analysis/semantic_analyzer.hpp"

namespace dao {
  class module {
    llvm::LLVMContext   &ctx_;
    llvm::IRBuilder<>   &builder_;
    llvm::TargetMachine *machine_;
    llvm::Module         mod_;

    dao::analysis_tables const &tables_;

    ankerl::unordered_dense::map<std::string, llvm::Value *> exports_;
    ankerl::unordered_dense::map<std::string, llvm::Value *> identifiers_;

    bool emit_main_;

  public:
    module(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder_,
      std::string const &source_fname, llvm::TargetMachine *target_machine,
      dao::analysis_tables const &tables, bool emit_main = false);

    [[nodiscard]]
    auto id() const -> std::string const &;

    auto print(llvm::raw_ostream &) const -> void;

    // Visitors
    auto operator()(dao::program_ast const &) -> llvm::Value *;
    auto operator()(dao::external_linkage_ast const &) -> llvm::Value *;
    auto operator()(dao::identifier_expr const &) -> llvm::Value *;
    auto operator()(dao::numeral_expr const &) -> llvm::Value *;
    auto operator()(dao::string_literal const &) -> llvm::Value *;
    auto operator()(dao::binary_expr const &) -> llvm::Value *;
    auto operator()(dao::function_proto const &) -> llvm::Value *;
    auto operator()(dao::function_def const &) -> llvm::Value *;
    auto operator()(dao::function_call const &) -> llvm::Value *;
    auto operator()(dao::if_expr const &) -> llvm::Value *;
    auto operator()(dao::type_alias const &) -> llvm::Value *;

    // Code Exporters
    auto emit_object_code() -> void;

  private:
    // Builders
    auto create_llvm_module(
      llvm::LLVMContext &ctx, std::string const &source_fname) -> llvm::Module;

    auto resolve_llvm_type(dao::type_expression const &expr) -> llvm::Type *;

    auto generate_function_prototype(dao::function_proto const &proto,
      llvm::Function::LinkageTypes linkages) -> llvm::Value *;
  };
} // namespace dao

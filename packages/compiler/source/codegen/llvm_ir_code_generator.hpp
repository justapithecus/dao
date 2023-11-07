#pragma once

#include <ankerl/unordered_dense.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include "../analysis/semantic_analyzer.hpp"

namespace dao {

  // TODO(andrew): may want to have a single top-level visitor that runs handler functions for
  //               semantic analysis and codegen
  class llvm_ir_code_generator {
    llvm::LLVMContext                    ctx_;
    // TODO(andrew): more than 1 main module
    llvm::Module                         mod_;
    llvm::IRBuilder<>                    builder_;
    std::unique_ptr<llvm::TargetMachine> machine_;

    dao::analysis_tables const                              &tables_;
    ankerl::unordered_dense::map<std::string, llvm::Value *> identifiers_;

  public:
    explicit llvm_ir_code_generator(
      std::string source_fname, dao::analysis_tables const &tables);

    auto generate(dao::ast const &) -> void;
    auto dumps() const -> std::string;

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

  private:
    auto resolve_llvm_type(std::optional<std::string> const &typename_)
      -> llvm::Type *;

    auto generate_function_prototype(dao::function_proto const &proto,
      llvm::Function::LinkageTypes linkages) -> llvm::Value *;

    auto emit_object_code() -> void;
  };
} // namespace dao

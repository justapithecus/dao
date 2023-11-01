#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include "../parser/ast.hpp"

namespace dao {

  class llvm_ir_code_generator {
    llvm::LLVMContext                    ctx_;
    // TODO(andrew): more than 1 main module
    llvm::Module                         mod_;
    llvm::IRBuilder<>                    builder_;
    std::unique_ptr<llvm::TargetMachine> machine_;

  public:
    explicit llvm_ir_code_generator(std::string source_fname);
    auto dumps() const -> std::string;
    auto emit_object_code();

    // Visitors
    auto operator()(dao::program const &) -> llvm::Value *;
    auto operator()(dao::identifier_expr const &) -> llvm::Value *;
    auto operator()(dao::numeral_expr const &) -> llvm::Value *;
    auto operator()(dao::binary_expr const &) -> llvm::Value *;
    auto operator()(dao::function_proto const &) -> llvm::Value *;
    auto operator()(dao::function_def const &) -> llvm::Value *;
    auto operator()(dao::function_call const &) -> llvm::Value *;
  };
} // namespace dao

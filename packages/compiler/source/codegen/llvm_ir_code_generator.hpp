#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

namespace dao {

  class llvm_ir_code_generator {
    llvm::LLVMContext                    ctx_;
    // TODO(andrew): more than 1 main module
    llvm::Module                         mod_;
    llvm::IRBuilder<>                    builder_;
    std::unique_ptr<llvm::TargetMachine> machine_;

  public:
    explicit llvm_ir_code_generator(std::string source_fname);
  };
} // namespace dao

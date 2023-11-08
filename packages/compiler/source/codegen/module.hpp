#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace dao {
  struct module {
    llvm::LLVMContext const &ctx_;
    llvm::Module             mod_;
  };
} // namespace dao

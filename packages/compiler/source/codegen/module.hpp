#pragma once

#include <ankerl/unordered_dense.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "../analysis/semantic_analyzer.hpp"

namespace dao {
  struct module {
    llvm::LLVMContext const &ctx_;
    llvm::Module             mod_;

    ankerl::unordered_dense::map<std::string, llvm::Value *> identifiers_;
    dao::analysis_tables const                              &tables_;
  };
} // namespace dao

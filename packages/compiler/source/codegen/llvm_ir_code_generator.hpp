#pragma once

#include "../analysis/semantic_analyzer.hpp"
#include "module.hpp"

namespace dao {

  // TODO(andrew): may want to have a single top-level visitor that runs handler functions for
  //               semantic analysis and codegen
  class llvm_ir_code_generator {
    llvm::LLVMContext    ctx_;
    llvm::IRBuilder<>    builder_;
    llvm::TargetMachine *machine_;

    ankerl::unordered_dense::map<std::string, module> modules_;
    dao::analysis_tables const                       &tables_;

  public:
    explicit llvm_ir_code_generator(dao::analysis_tables const &tables);

    [[nodiscard]]
    auto generate(std::string const &source_fname, dao::ast const &,
      bool                           emit_main = false) -> std::string const &;

    auto emit(std::string const &source_fname, dao::ast const &,
      bool                       emit_main = false) -> void;

    auto dumps(std::string const &source_fname) const -> std::string;

  private:
    auto create_target_machine() -> llvm::TargetMachine *;

    auto resolve_llvm_type(dao::type_expression const &expr) -> llvm::Type *;

    auto generate_function_prototype(dao::function_proto const &proto,
      llvm::Function::LinkageTypes linkages) -> llvm::Value *;
  };
} // namespace dao

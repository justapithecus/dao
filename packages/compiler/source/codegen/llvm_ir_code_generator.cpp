#include "llvm_ir_code_generator.hpp"

#include <iostream>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>

namespace dao {

  auto set_target_machine(llvm::Module &mod)
    -> std::unique_ptr<llvm::TargetMachine> {

    // TODO(andrew): probably don't want to do this, and use target platform unless specified
    //               through environments or cli flags
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string error{};

    auto target_triple{llvm::sys::getDefaultTargetTriple()};
    auto target{llvm::TargetRegistry::lookupTarget(target_triple, error)};

    if (not target) {
      std::cerr << "failed to determine target: " << error << std::endl;
      std::exit(1); // TODO(andrew): exceptions
    }

    auto constexpr cpu{"generic"};
    auto constexpr features{""};
    auto options{llvm::TargetOptions{}};
    auto reloc_model{
      std::optional<llvm::Reloc::Model>{llvm::Reloc::Model::PIC_}};
    auto target_machine{target->createTargetMachine(
      target_triple, cpu, features, options, reloc_model)};

    return std::unique_ptr<llvm::TargetMachine>(target_machine);
  }

  llvm_ir_code_generator::llvm_ir_code_generator(std::string source_fname)
    : ctx_{}
    , mod_{"main", ctx_}
    , builder_{ctx_}
    , machine_{set_target_machine(mod_)} {

    mod_.setSourceFileName(std::move(source_fname));
  }

} // namespace dao

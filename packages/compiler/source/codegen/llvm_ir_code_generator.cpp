#include "llvm_ir_code_generator.hpp"

#include <iostream>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>

namespace dao {

  llvm_ir_code_generator::llvm_ir_code_generator(
    dao::analysis_tables const &tables)
    : ctx_{}
    , builder_{ctx_}
    , machine_{create_target_machine()}
    , modules_{}
    , tables_{tables} {
  }

  auto llvm_ir_code_generator::generate(std::string const &source_fname,
    dao::ast const &ast, bool emit_main) -> std::string const & {
    auto mod{
      module{ctx_, builder_, source_fname, machine_, tables_, emit_main}};
    std::visit(mod, ast);
    modules_.emplace(mod.id(), std::move(mod));
    return mod.id();
  }

  auto llvm_ir_code_generator::emit(std::string const &source_fname,
    dao::ast const &ast, bool emit_main) -> void {
    auto  mod_id{generate(source_fname, ast, emit_main)};
    auto &mod{modules_.at(mod_id)};
    mod.emit_object_code();
  }

  auto llvm_ir_code_generator::dumps(std::string const &id) const
    -> std::string {
    std::string              str{};
    llvm::raw_string_ostream ss{str};
    auto const              &mod{modules_.at(id)};
    mod.print(ss);
    return ss.str();
  }

  //---------------------------------------------------------------------------
  // Private Methods
  //---------------------------------------------------------------------------
  auto llvm_ir_code_generator::create_target_machine()
    -> llvm::TargetMachine * {
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
    return target->createTargetMachine(
      target_triple, cpu, features, options, reloc_model);
  }

} // namespace dao

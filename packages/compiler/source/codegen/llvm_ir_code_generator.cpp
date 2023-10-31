#include "llvm_ir_code_generator.hpp"

#include <iostream>
#include <llvm/IR/LegacyPassManager.h>
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

  auto llvm_ir_code_generator::dumps() const -> std::string {
    std::string              str{};
    llvm::raw_string_ostream ss{str};
    mod_.print(ss, nullptr);
    return ss.str();
  }

  auto llvm_ir_code_generator::emit_object_code() {
    auto            obj_fname{mod_.getSourceFileName() + ".o"};
    std::error_code ec;

    llvm::raw_fd_ostream dest{obj_fname, ec};

    // TODO(andrew): see new pass managers
    llvm::legacy::PassManager pass{};

    auto file_type{llvm::CodeGenFileType::CGFT_ObjectFile};
    if (machine_->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
      std::exit(1); // TODO(andrew): exceptions
    }

    pass.run(mod_);
    dest.flush();
  }

  //---------------------------------------------------------------------------
  // Visitors
  //---------------------------------------------------------------------------
  auto llvm_ir_code_generator::operator()(dao::identifier_expr const &expr)
    -> void {
  }

  auto llvm_ir_code_generator::operator()(dao::numeral_expr const &expr)
    -> void {
  }

  auto llvm_ir_code_generator::operator()(dao::binary_expr const &expr)
    -> void {
  }

  auto llvm_ir_code_generator::operator()(dao::function_proto const &proto)
    -> void {
  }

  auto llvm_ir_code_generator::operator()(dao::function_def const &def)
    -> void {
    if (def.proto.id == "main") {
      auto main_ft{llvm::FunctionType::get(builder_.getInt32Ty(), false)};
      auto main{llvm::Function::Create(
        main_ft, llvm::Function::ExternalLinkage, "main", mod_)};

      builder_.SetInsertPoint(llvm::BasicBlock::Create(ctx_, "entry", main));

      std::visit(*this, *(def.body));

      auto constexpr int_size{32};
      auto constexpr is_signed{true};
      auto ret{llvm::APInt{int_size, 0, is_signed}};
      builder_.CreateRet(llvm::ConstantInt::get(ctx_, ret));
    }
  }

  auto llvm_ir_code_generator::operator()(dao::function_call const &call)
    -> void {
  }

} // namespace dao

#include "llvm_ir_code_generator.hpp"

#include <iostream>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
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

    mod.setTargetTriple(target_triple);
    mod.setDataLayout(target_machine->createDataLayout());
    return std::unique_ptr<llvm::TargetMachine>(target_machine);
  }

  llvm_ir_code_generator::llvm_ir_code_generator(std::string source_fname)
    : ctx_{}
    , mod_{"main", ctx_}
    , builder_{ctx_}
    , machine_{set_target_machine(mod_)}
    , identifiers_{} {

    mod_.setSourceFileName(std::move(source_fname));
  }

  auto llvm_ir_code_generator::generate(dao::ast const &ast) -> void {
    std::visit(*this, ast);
    emit_object_code();
  }

  auto llvm_ir_code_generator::dumps() const -> std::string {
    std::string              str{};
    llvm::raw_string_ostream ss{str};
    mod_.print(ss, nullptr);
    return ss.str();
  }

  //---------------------------------------------------------------------------
  // Visitors
  //---------------------------------------------------------------------------
  auto llvm_ir_code_generator::operator()(dao::program_ast const &prog)
    -> llvm::Value * {
    // build program definitions
    std::for_each(prog.nodes.begin(), prog.nodes.end(),
      [this](auto &&node) { std::visit(*this, *node); });

    // build program entrypoint
    auto ft{llvm::FunctionType::get(builder_.getInt32Ty(), false)};
    auto fn_decl{llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, "main", mod_)};

    auto basic_block{llvm::BasicBlock::Create(ctx_, "entry", fn_decl)};
    builder_.SetInsertPoint(basic_block);

    std::for_each(prog.entry.begin(), prog.entry.end(),
      [this](auto &&node) { std::visit(*this, *node); });

    auto constexpr int_size{32};
    auto constexpr is_signed{true};
    auto ret{llvm::APInt{int_size, 0, is_signed}};
    return builder_.CreateRet(llvm::ConstantInt::get(ctx_, ret));
  }

  auto llvm_ir_code_generator::operator()(dao::external_linkage_ast const &link)
    -> llvm::Value * {
    return nullptr;
  }

  auto llvm_ir_code_generator::operator()(dao::identifier_expr const &expr)
    -> llvm::Value * {
    // TODO(andrew): lookup needs to be scoped!
    if (auto val{identifiers_.find(expr.name)}; val != identifiers_.end()) {
      return val->second;
    }
    // TODO(andrew): errors
    return nullptr;
  }

  auto llvm_ir_code_generator::operator()(dao::numeral_expr const &expr)
    -> llvm::Value * {
    return llvm::ConstantFP::get(
      ctx_, llvm::APFloat(llvm::APFloat::IEEEdouble(), expr.val));
  }

  auto llvm_ir_code_generator::operator()(dao::string_literal const &litr)
    -> llvm::Value * {
    return llvm::ConstantDataArray::getString(ctx_, litr.val);
  }

  auto llvm_ir_code_generator::operator()(dao::binary_expr const &expr)
    -> llvm::Value * {
    auto lhs{std::visit(*this, *(expr.lhs))};
    auto rhs{std::visit(*this, *(expr.rhs))};

    // TODO(andrew): lhs vs. rhs type checking
    switch (expr.op) {
    case '+':
      return builder_.CreateFAdd(lhs, rhs, "addtmp");
    case '-':
      return builder_.CreateFSub(lhs, rhs, "subtmp");
    case '*':
      return builder_.CreateFMul(lhs, rhs, "multmp");
    case '/':
      return builder_.CreateFDiv(lhs, rhs, "divtmp");
    case '<':
      return builder_.CreateFCmpULT(lhs, rhs, "lttmp");
    case '>':
      return builder_.CreateFCmpUGT(lhs, rhs, "gttmp");
    default:
      // TODO(andrew): errors
      return nullptr;
    }
  }

  auto llvm_ir_code_generator::operator()(dao::function_proto const &proto)
    -> llvm::Value * {
    std::vector<llvm::Type *> arg_types{
      proto.args.size(), builder_.getDoubleTy()};

    auto constexpr is_var_arg{false};
    auto ft{
      llvm::FunctionType::get(builder_.getDoubleTy(), arg_types, is_var_arg)};
    auto fn{llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, proto.id, mod_)};

    // TODO(andrew): ranges
    unsigned int i{0};
    for (auto &arg : fn->args()) {
      arg.setName(proto.args[i++].name);
    }
    return fn;
  }

  auto llvm_ir_code_generator::operator()(dao::function_def const &def)
    -> llvm::Value * {
    auto fn_decl{static_cast<llvm::Function *>(
      std::visit(*this, static_cast<ast>(def.proto)))};

    auto basic_block{llvm::BasicBlock::Create(ctx_, "entry", fn_decl)};
    builder_.SetInsertPoint(basic_block);

    // TODO(andrew): identifiers_ should be scoped, instead of being reset each time
    identifiers_.clear();
    for (auto &arg : fn_decl->args()) {
      identifiers_[std::string{arg.getName()}] = &arg;
    }

    if (auto ret{std::visit(*this, *(def.body))}; ret) {
      builder_.CreateRet(ret);

      llvm::verifyFunction(*fn_decl, &llvm::errs());
      return fn_decl;
    }

    // TODO(andrew): errors
    fn_decl->eraseFromParent();
    return nullptr;
  }

  auto llvm_ir_code_generator::operator()(dao::function_call const &call)
    -> llvm::Value * {
    auto callee{mod_.getFunction(call.callee)};
    if (!callee) {
      // TODO(andrew): errors, referencing undefined function
      return nullptr;
    }

    if (callee->arg_size() != call.args.size()) {
      // TODO(andrew): errors, mismatched number of args
      return nullptr;
    }

    std::vector<llvm::Value *> args{};
    args.reserve(callee->arg_size());
    std::for_each(
      call.args.begin(), call.args.end(), [this, &args](auto &&arg) {
        if (auto node{std::visit(*this, *arg)}; node) {
          args.emplace_back(std::move(node));
        } else {
          // TODO(andrew): collect errors
        }
      });
    return builder_.CreateCall(callee, args, "calltmp");
  }

  //---------------------------------------------------------------------------
  // Private Methods
  //---------------------------------------------------------------------------
  auto llvm_ir_code_generator::emit_object_code() -> void {
    auto            obj_fname{mod_.getSourceFileName() + ".o"};
    std::error_code ec;

    // TODO(andrew): emit to any ostream and let caller decide type of sink
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

} // namespace dao

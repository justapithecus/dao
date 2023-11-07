#include "llvm_ir_code_generator.hpp"

#include <iostream>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
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

  llvm_ir_code_generator::llvm_ir_code_generator(
    std::string source_fname, dao::analysis_tables const &tables)
    : ctx_{}
    , mod_{"main", ctx_}
    , builder_{ctx_}
    , machine_{set_target_machine(mod_)}
    , tables_{tables}
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
    // TODO(andrew): name mangler depending on linkage type
    return generate_function_prototype(
      link.proto, llvm::Function::ExternalLinkage);
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
    return builder_.CreateGlobalString(litr.val);
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
    return generate_function_prototype(proto, llvm::Function::InternalLinkage);
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

  auto llvm_ir_code_generator::operator()(dao::if_expr const &expr)
    -> llvm::Value * {
    auto cond_value{std::visit(*this, *(expr.cond_))};
    if (not cond_value) {
      // TODO(andrew): ctx.errors
      return nullptr;
    }

    if (not cond_value->getType()->isIntegerTy()) {
      // convert condition to bool (i1 type) by comparing neq to 0.0
      cond_value = builder_.CreateFCmpONE(
        cond_value, llvm::ConstantFP::get(ctx_, llvm::APFloat(0.0)), "ifcond");
    }

    auto function{builder_.GetInsertBlock()->getParent()};
    auto then_bb{llvm::BasicBlock::Create(ctx_, "then", function)};
    auto else_bb{llvm::BasicBlock::Create(ctx_, "else")};
    auto merge_bb{llvm::BasicBlock::Create(ctx_, "ifcont")};

    builder_.CreateCondBr(cond_value, then_bb, else_bb);

    // emit then value
    builder_.SetInsertPoint(then_bb);
    auto then_value{std::visit(*this, *(expr.then_))};
    if (not then_value) {
      // TODO(andrew): ctx.errors
      return nullptr;
    }

    builder_.CreateBr(merge_bb);
    then_bb = builder_.GetInsertBlock();

    // emit else block
    llvm::Value *else_value{};
    if (expr.else_) {
      function->insert(function->end(), else_bb);
      builder_.SetInsertPoint(else_bb);

      else_value = std::visit(*this, *(expr.else_));
      if (not else_value) {
        // TODO(andrew): ctx.errors
        return nullptr;
      }

      builder_.CreateBr(merge_bb);
      else_bb = builder_.GetInsertBlock();
    }

    // emit merge block
    function->insert(function->end(), merge_bb);
    builder_.SetInsertPoint(merge_bb);

    // TODO(andrew): phi_node type and addIncoming type need to match,
    //               so then_value.getType() must equal else_value.getType()?
    auto phi_node{builder_.CreatePHI(then_value->getType(), 2, "iftmp")};

    phi_node->addIncoming(then_value, then_bb);
    if (else_value and else_bb) {
      phi_node->addIncoming(else_value, else_bb);
    }
    return phi_node;
  }

  auto llvm_ir_code_generator::operator()(dao::type_alias const &)
    -> llvm::Value * {
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Private Methods
  //---------------------------------------------------------------------------
  auto llvm_ir_code_generator::resolve_llvm_type(
    std::optional<std::string> const &typename_) -> llvm::Type * {
    if (typename_.has_value()) {
      auto resolved{typename_.value()};

      // resolve to builtin
      if (auto it{tables_.types_.find(resolved)}; it != tables_.types_.end()) {
        resolved = it->second;
      }

      // convert builtin type to LLVM type
      if (auto it{builtin_types.find(resolved)}; it != builtin_types.end()) {
        switch (it->second) {
        case builtin_type_kind::e_int8:
          // TODO(andrew): placeholder and actually incorrect until ptr types are introduced
          return builder_.getInt8PtrTy();
        case builtin_type_kind::e_int32:
          return builder_.getInt32Ty();
        }
      }
    }
    // for now, default to double when arg type is not specified, i.e. deduced
    // collect error when type cannot be deduced
    return builder_.getDoubleTy();
  }

  auto llvm_ir_code_generator::generate_function_prototype(
    function_proto const &proto, llvm::Function::LinkageTypes linkages)
    -> llvm::Value * {
    std::vector<llvm::Type *> arg_types{};
    arg_types.reserve(proto.args.size());
    std::transform(proto.args.begin(), proto.args.end(),
      std::back_inserter(arg_types), [this](auto &&arg) -> llvm::Type * {
        return resolve_llvm_type(arg.typename_);
      });

    // TODO(andrew): implement return type resolution
    auto return_type{resolve_llvm_type(proto.ret)};
    auto constexpr is_var_arg{false};
    auto ft{llvm::FunctionType::get(return_type, arg_types, is_var_arg)};
    auto fn{llvm::Function::Create(ft, linkages, proto.id, mod_)};

    // TODO(andrew): ranges
    unsigned int i{0};
    for (auto &arg : fn->args()) {
      arg.setName(proto.args[i++].name);
    }
    return fn;
  }

  auto llvm_ir_code_generator::emit_object_code() -> void {
    auto            obj_fname{mod_.getSourceFileName() + ".o"};
    std::error_code ec;

    // TODO(andrew): emit to any ostream and let caller decide type of sink
    llvm::raw_fd_ostream dest{obj_fname, ec, llvm::sys::fs::OF_None};
    if (ec) {
      std::cerr << "could not open file: " << ec.message() << std::endl;
      std::exit(1);
    }

    // TODO(andrew): see new pass managers
    llvm::legacy::PassManager pass{};

    auto file_type{llvm::CodeGenFileType::CGFT_ObjectFile};
    auto constexpr disable_verify{false};
    if (machine_->addPassesToEmitFile(
          pass, dest, nullptr, file_type, disable_verify)) {
      std::cerr << "failed to emit passes" << std::endl;
      std::exit(1); // TODO(andrew): exceptions
    }

    pass.run(mod_);
    dest.flush();
  }

} // namespace dao

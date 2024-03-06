#include "module.hpp"

#include <filesystem>
#include <iostream>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>

namespace dao {

  module::module(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder_,
    std::string const &source_fname, llvm::TargetMachine *target_machine,
    dao::analysis_tables const &tables, bool emit_main) :ctx_{ctx},
    builder_{builder_}, machine_{target_machine},
    mod_{create_llvm_module(ctx_, source_fname)}, tables_{tables},
    emit_main_{emit_main} {
    mod_.setTargetTriple(machine_->getTargetTriple().getTriple());
    mod_.setDataLayout(machine_->createDataLayout());
  }

  auto module::id() const -> std::string const & {
    return mod_.getModuleIdentifier();
  }

  auto module::print(llvm::raw_ostream &os) const -> void {
    return mod_.print(os, nullptr);
  }

  //---------------------------------------------------------------------------
  // Visitors
  //---------------------------------------------------------------------------
  auto module::operator()(dao::program_ast const &prog) -> llvm::Value * {
    // build program definitions
    std::for_each(prog.nodes.begin(), prog.nodes.end(),
      [this](auto &&node) { std::visit(*this, *node); });

    if (not emit_main_) {
      return nullptr;
    }

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

  auto module::operator()(dao::external_linkage_ast const &link)
    -> llvm::Value * {
    // TODO(andrew): name mangler depending on linkage type
    return generate_function_prototype(
      link.proto, llvm::Function::ExternalLinkage);
  }

  auto module::operator()(dao::identifier_expr const &expr) -> llvm::Value * {
    // TODO(andrew): lookup needs to be scoped!
    if (auto val{identifiers_.find(expr.name)}; val != identifiers_.end()) {
      return val->second;
    }
    // TODO(andrew): errors
    return nullptr;
  }

  auto module::operator()(dao::numeral_expr const &expr) -> llvm::Value * {
    return llvm::ConstantFP::get(
      ctx_, llvm::APFloat(llvm::APFloat::IEEEdouble(), expr.val));
  }

  auto module::operator()(dao::string_literal const &litr) -> llvm::Value * {
    return builder_.CreateGlobalString(litr.val);
  }

  auto module::operator()(dao::binary_expr const &expr) -> llvm::Value * {
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

  auto module::operator()(dao::function_proto const &proto) -> llvm::Value * {
    return generate_function_prototype(proto, llvm::Function::InternalLinkage);
  }

  auto module::operator()(dao::function_def const &def) -> llvm::Value * {
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

      // TODO(andrew): errors
      // fn_decl->eraseFromParent();
      return fn_decl;
    }

    return nullptr;
  }

  auto module::operator()(dao::function_call const &call) -> llvm::Value * {
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

  auto module::operator()(dao::if_expr const &expr) -> llvm::Value * {
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

  auto module::operator()(dao::type_alias const &) -> llvm::Value * {
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Code Exporters
  //---------------------------------------------------------------------------
  auto module::emit_object_code() -> void {
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

  //---------------------------------------------------------------------------
  // Private Methods
  //---------------------------------------------------------------------------
  auto module::create_llvm_module(
    llvm::LLVMContext &ctx, std::string const &source_fname) -> llvm::Module {
    auto id{std::filesystem::path{source_fname}
              .filename()
              .replace_extension("")
              .generic_string()};
    return llvm::Module{id, ctx};
  }

  //---------------------------------------------------------------------------
  // Builders
  //---------------------------------------------------------------------------
  auto module::resolve_llvm_type(dao::type_expression const &expr)
    -> llvm::Type * {
    if (expr.typename_.has_value()) {
      auto resolved{expr.typename_.value()};

      // resolve to builtin
      if (auto it{tables_.types_.find(resolved)}; it != tables_.types_.end()) {
        resolved = it->second;
      }

      // convert builtin type to LLVM type
      if (auto it{builtin_types.find(resolved)}; it != builtin_types.end()) {
        auto is_pointer{expr.declarator == type_declarator::e_pointer};

        switch (it->second) {
        case builtin_type_kind::e_int8:
          return is_pointer ? static_cast<llvm::Type *>(builder_.getInt8PtrTy())
                            : builder_.getInt8Ty();
        case builtin_type_kind::e_int32:
          return is_pointer
                   ? static_cast<llvm::Type *>(llvm::Type::getInt32PtrTy(ctx_))
                   : builder_.getInt32Ty();
        }
      }
    }
    // for now, default to double when arg type is not specified, i.e. deduced
    // collect error when type cannot be deduced
    return builder_.getDoubleTy();
  }

  auto module::generate_function_prototype(function_proto const &proto,
    llvm::Function::LinkageTypes linkages) -> llvm::Value * {
    std::vector<llvm::Type *> arg_types{};
    arg_types.reserve(proto.args.size());
    std::transform(proto.args.begin(), proto.args.end(),
      std::back_inserter(arg_types), [this](auto &&arg) -> llvm::Type * {
        return resolve_llvm_type(arg.type_expr);
      });

    auto constexpr is_var_arg{false};
    auto return_type{resolve_llvm_type(proto.ret)};
    auto ft{llvm::FunctionType::get(return_type, arg_types, is_var_arg)};
    auto fn{llvm::Function::Create(ft, linkages, proto.id, mod_)};

    // TODO(andrew): ranges
    unsigned int i{0};
    for (auto &arg : fn->args()) {
      arg.setName(proto.args[i++].name);
    }
    return fn;
  }

} // namespace dao

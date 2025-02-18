#include "ast_pretty_printer.hpp"

namespace dao {
  auto ast_pretty_printer::operator()(dao::program_ast const &prog) const
    -> json {
    return json{
      {"type", "program"},
      {
        "value",
        {
          {"entry", prog.entry},
          {"meta", prog.meta},
          {"nodes", prog.nodes},
        },
      },
    };
  };

  std::string const linkage_kinds[]{"C"};

  auto ast_pretty_printer::operator()(
    dao::external_linkage_ast const &link) const -> json {
    return json{
      {"type", "external_linkage"},
      {
        "value",
        {
          {"kind", linkage_kinds[static_cast<int>(link.kind)]},
          {"prototype", link.proto},
        },
      },
    };
  }

  auto ast_pretty_printer::operator()(dao::identifier_expr const &expr) const
    -> json {
    return json{
      {"type", "identifier_expression"},
      {"value", expr.name},
    };
  }

  auto ast_pretty_printer::operator()(dao::numeral_expr const &expr) const
    -> json {
    return json{
      {"type", "numeral_expression"},
      {"value", expr.val},
    };
  }

  auto ast_pretty_printer::operator()(dao::string_literal const &litr) const
    -> json {
    return json{
      {"type", "string_literal"},
      {"value", litr.val},
    };
  }

  auto ast_pretty_printer::operator()(dao::binary_expr const &expr) const
    -> json {
    auto lhs{std::visit(*this, *(expr.lhs))};
    json rhs{};
    if (expr.rhs) {
      rhs = std::visit(*this, *(expr.rhs));
    } else {
      rhs = nullptr;
    }

    return json{
      {"type", "binary_expression"},
      {
        "value",
        {
          {"lhs", lhs},
          {"rhs", rhs},
          {"operand", std::string{expr.op}},
        },
      },
    };
  }

  auto ast_pretty_printer::operator()(dao::function_proto const &expr) const
    -> json {
    return json{
      {"type", "function_prototype"},
      {"value", expr},
    };
  }

  auto ast_pretty_printer::operator()(dao::function_def const &expr) const
    -> json {
    auto body{std::visit(*this, *(expr.body))};
    return json{
      {"type", "function_definition"},
      {
        "value",
        {
          {"prototype", expr.proto},
          {"body", body},
        },
      },
    };
  }

  auto ast_pretty_printer::operator()(dao::function_call const &call) const
    -> json {
    return json{
      {"type", "function_call"},
      {
        "value",
        {
          {"callee", call.callee},
          {"args", call.args},
        },
      },
    };
  }

  auto ast_pretty_printer::operator()(dao::if_expr const &expr) const -> json {
    return json{
      {"type", "if_then_else_expression"},
      {
        "value",
        {
          {"condition", expr.cond_},
          {"then", expr.then_},
          {"else", expr.else_},
        },
      },
    };
  }

  auto ast_pretty_printer::operator()(dao::type_alias const &meta) const
    -> json {
    return json{
      {"type", "type_alias"},
      {
        "value",
        {
          {"from", meta.from},
          {"to", meta.to},
        },
      },
    };
  }

} // namespace dao

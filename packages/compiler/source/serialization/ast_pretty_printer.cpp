#include "ast_pretty_printer.hpp"

namespace dao {

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
      {
        "value",
        {
          {"id", expr.id},
          {"args", expr.args},
        },
      },
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

  auto ast_pretty_printer::operator()(dao::function_call const &expr) const
    -> json {
    return json{
      {"type", "function_call"},
      {
        "value",
        {
          {"callee", expr.callee},
          {"args", expr.args},
        },
      },
    };
  }

} // namespace dao

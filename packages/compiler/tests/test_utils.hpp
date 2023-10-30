#pragma once

#include <ankerl/unordered_dense.h>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <type_traits>
#include <variant>

#include "fixtures.hpp"
#include "parser/ast.hpp"
#include "parser/state_machine.h"
#include "parser/token.hpp"

using json = nlohmann::json;

#ifdef CTEST

#define token_desc(kind, desc)                                                 \
  case (kind):                                                                 \
    os << (desc);                                                              \
    break

namespace dao {

  inline auto operator<<(std::ostream &os, dao::token const &tok)
    -> std::ostream & {
    os << std::setw(12);

    switch (tok.kind) {
      token_desc(token_kind::e_identifier, "Identifier");
      token_desc(token_kind::e_numeral, "Numeral");
      token_desc(token_kind::e_operator, "Operator");
      token_desc(token_kind::e_separator, "Separator");
      token_desc(token_kind::e_keyword, "Keyword");
      os << "Unknown";
      break;
    }

    return os << " | " << tok.repr << " |";
  }

  inline ankerl::unordered_dense::map<std::string, dao::token_kind>
    str_to_kind = {
      {"identifier", token_kind::e_identifier},
      {"numeral", token_kind::e_numeral},
      {"operator", token_kind::e_operator},
      {"separator", token_kind::e_separator},
      {"keyword", token_kind::e_keyword},
  };

  inline auto to_json(json &j, dao::token const &tok) {
    j = json{{"kind", tok.kind}, {"repr", tok.repr}};
  }

  inline auto from_json(json const &j, dao::token &tok) {
    std::string kind{};
    j.at("kind").get_to(kind);
    j.at("repr").get_to(tok.repr);

    tok.kind = str_to_kind.at(kind);
  }

  inline auto to_json(json &j, dao::function_arg const &arg) {
    j = json{{"name", arg.name}};
  }

  inline auto from_json(json const &j, dao::function_arg &arg) {
  }

  inline auto to_json(json &j, dao::ast const &node) -> void {
    // TODO(andrew): extract to visitor pattern

    std::visit(
      [&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<dao::identifier_expr, T>) {
          j = json{
            {"type", "identifier_expr"},
            {"value", std::get<dao::identifier_expr>(node).name},
          };
        } else if constexpr (std::is_same_v<dao::numeral_expr, T>) {
          j = json{
            {"type", "numeral_expr"},
            {"value", std::get<dao::numeral_expr>(node).val},
          };
        } else if constexpr (std::is_same_v<dao::binary_expr, T>) {
          auto &expr{std::get<dao::binary_expr>(node)};
          json  lhs{}, rhs{};

          to_json(lhs, *(expr.lhs));
          if (expr.rhs) {
            to_json(rhs, *(expr.rhs));
          } else {
            rhs = nullptr;
          }

          json value{
            {"lhs", lhs},
            {"rhs", rhs},
            {"operand", std::string{expr.op}},
          };

          j = json{
            {"type", "binary_expr"},
            {"value", value},
          };
        } else if constexpr (std::is_same_v<dao::function_proto, T>) {
          auto &proto{std::get<dao::function_proto>(node)};

          json value{
            {"id", proto.id},
            {"args", proto.args},
          };

          j = json{
            {"type", "function_proto"},
            {"value", value},
          };
        }
      },
      node);
  }

  inline auto from_json(json const &j, dao::ast &node) {
    // j.at("number_expr").get_to(ast.val);
  }

} // namespace dao

namespace nlohmann {
  template <typename T>
  struct adl_serializer<std::unique_ptr<T>> {
    static auto to_json(json &j, std::unique_ptr<T> const &opt) -> void {
      if (opt.get()) {
        j = *opt;
      } else {
        j = nullptr;
      }
    }
  };
} // namespace nlohmann

template <typename T>
class json_writer : public ApprovalWriter {
  T contents_;

public:
  explicit json_writer(T contents)
    : contents_{std::move(contents)} {
  }

  auto getFileExtensionWithDot() const -> std::string override {
    return ".json";
  }

  auto write(std::string path) const -> void override {
    std::ofstream ofs{path.c_str()};
    if (ofs) {
      write(ofs);
      ofs.close();
    } else {
      throw std::runtime_error{"unable to write file: " + path};
    }
  }

  auto write(std::ostream &out) const {
    json j{{"root", contents_}};
    out << j.dump(2) << "\n";
  }

  auto cleanUpReceived(std::string received_path) const -> void override {
    ::remove(received_path.c_str());
  }
};

inline auto load_tokens(std::string const &fname) {
  std::ifstream ifs{"packages/compiler/tests/data/" + fname};
  if (ifs) {
    auto data = json::parse(ifs);
    return data.at("tokens").get<std::vector<dao::token>>();
  }

  throw std::runtime_error{"failed to open data/" + fname};
}

#endif

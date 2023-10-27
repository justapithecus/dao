#pragma once

#include <nlohmann/json.hpp>
#include <type_traits>
#include <variant>

#include "fixtures.hpp"
#include "parser/ast.hpp"
#include "parser/state_machine.h"
#include "parser/token.hpp"

using json = nlohmann::json;

namespace dao {

  inline std::unordered_map<std::string, token_kind> str_to_kind = {
    {"identifier", token_kind_identifier},
    {"numeral", token_kind_numeral},
  };

  inline auto to_json(json &j, dao::token const &tok) {
    j = json{{"kind", tok.kind}, {"repr", tok.repr}};
  }

  inline auto from_json(json const &j, dao::token &tok) {
    std::string kind;
    j.at("kind").get_to(kind);
    j.at("repr").get_to(tok.repr);

    tok.kind = str_to_kind.at(kind);
  }

  inline auto to_json(json &j, dao::ast_node const &node) {
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
        }
      },
      node);
  }

  inline auto from_json(json const &j, dao::ast_node &node) {
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

  auto cleanUpReceived(std::string receivedPath) const -> void override {
    ::remove(receivedPath.c_str());
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

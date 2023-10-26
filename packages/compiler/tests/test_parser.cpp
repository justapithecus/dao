#include <nlohmann/json.hpp>

#include "fixtures.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"
#include "parser/token.hpp"

using json = nlohmann::json;

auto directory = Approvals::useApprovalsSubdirectory("golden-files");

namespace dao {
  auto to_json(json &j, dao::token const &tok) {
    j = json{{"kind", tok.kind}, {"repr", tok.repr}};
  }

  auto from_json(json const &j, dao::token &tok) {
    j.at("kind").get_to(tok.kind);
    j.at("repr").get_to(tok.repr);
  }
} // namespace dao

auto load_tokens(std::string const &fname) {
  std::ifstream ifs{"data/" + fname};
  auto          data = json::parse(ifs);
  return data.at("tokens").get<std::vector<dao::token>>();
}

auto main() -> int {
  "simple_identifier_expression"_test = [] {
    auto tokens = load_tokens("simple_identifier_expression.json");
    auto ast    = dao::parse(tokens);
    // auto data   = json{"root", ast};
  };
}

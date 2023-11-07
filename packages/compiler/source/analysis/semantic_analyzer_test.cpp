#include "../../tests/test_utils.hpp"

#include "../parser/parser.hpp"
#include "semantic_analyzer.hpp"

auto main() -> int {

  // use approved parser outputs as inputs to semantic analyzer
  for (auto const &entry : load_test_cases("golden-files/json")) {
    auto path{entry.path()};
    auto name{path.stem().generic_string()};
    if (entry.is_regular_file() and
        name.find("tokens.approved") != std::string::npos) {
      auto base_name{path.stem().stem().stem().generic_string()};
      auto namer{TemplatedCustomNamer::create(
        std::string{test_path} + "golden-files/tables/" +
        std::string{base_name} + ".{ApprovedOrReceived}.{FileExtension}")};

      // TODO(andrew): load AST from json directly
      auto tokens{load_tokens(path.generic_string())};
      auto ast{dao::parser{tokens}.parse()};

      // for now, skip files where prog.meta is empty
      if (auto const &prog{std::get<dao::program_ast>(ast)};
          prog.meta.empty()) {
        continue;
      }

      test(name) = [&, ast = std::move(ast)] {
        auto analyzer{dao::semantic_analyzer{}};
        std::visit(analyzer, std::move(ast));

        json contents{
          {"_filename", base_name + ".dao"},
          {
            "metatables",
            {"types", analyzer.dump()},
          },
        };

        Approvals::verify(json_writer{contents}, Options().withNamer(namer));
      };
    }
  }
}

#include "../../tests/test_utils.hpp"

#include "../parser/parser.hpp"
#include "semantic_analyzer.hpp"

auto main() -> int {

  // use approved parser outputs as inputs to semantic analyzer
  for (auto const &entry : load_test_cases("golden-files")) {
    auto path{entry.path()};
    auto name{path.stem().generic_string()};
    if (entry.is_regular_file() and name.find("tokens") != std::string::npos) {
      auto filename{path.filename().generic_string()};
      auto base_name{path.stem().stem().stem().generic_string()};
      auto namer{TemplatedCustomNamer::create(
        std::string{test_path} + "golden-files/tables/" +
        std::string{base_name} + ".{ApprovedOrReceived}.{FileExtension}")};

      test(name) = [&] {
        // TODO(andrew): load AST from json directly
        auto tokens{load_tokens(path.generic_string())};
        auto analyzer{dao::semantic_analyzer{}};
        auto ast{dao::parser{tokens}.parse()};
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

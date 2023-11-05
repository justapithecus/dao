#include "../../tests/test_utils.hpp"

#include "parser/parser.hpp"
#include "parser/state_machine.h"

auto directory{
  Approvals::useApprovalsSubdirectory("../../tests/golden-files/ast")};

auto main() -> int {

  for (auto const &entry : load_test_cases_tokens()) {
    auto path{entry.path()};
    auto name{path.stem().generic_string()};
    if (entry.is_regular_file() and name.find("tokens") != std::string::npos) {
      auto filename{path.filename().generic_string()};
      auto base_name{path.stem().stem().stem().generic_string()};
      auto namer{TemplatedCustomNamer::create(
        std::string{test_path} + "golden-files/" + std::string{base_name} +
        ".ast.{ApprovedOrReceived}.{FileExtension}")};

      test(name) = [&] {
        auto tokens{load_tokens(path.generic_string())};
        json contents{
          {"_filename", filename},
          {"ast", dao::parser{tokens}.parse()},
        };

        Approvals::verify(json_writer{contents}, Options().withNamer(namer));
      };
    }
  }
}

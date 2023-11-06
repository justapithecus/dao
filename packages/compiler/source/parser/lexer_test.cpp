#include "../../tests/test_utils.hpp"

#include "parser/lexer.hpp"

auto main() -> int {

  for (auto const &file : load_test_cases("cases")) {
    auto path{file.path()};
    auto name{path.stem().generic_string()};
    auto filename{path.filename().generic_string()};
    auto namer{TemplatedCustomNamer::create(
      std::string{test_path} + "golden-files/" + std::string{name} +
      ".tokens.{ApprovedOrReceived}.{FileExtension}")};

    test(name) = [&] {
      json contents{
        {"_filename", filename},
        {"tokens", dao::lex(path.c_str())},
      };

      Approvals::verify(json_writer{contents}, Options().withNamer(namer));
    };
  }
}

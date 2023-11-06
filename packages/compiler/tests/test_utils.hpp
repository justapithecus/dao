#pragma once
#ifdef CTEST

#include <filesystem>

#include "fixtures.hpp"
#include "serialization/ast_pretty_printer.hpp"
#include "serialization/token.hpp"

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
    json j(contents_);
    out << j.dump(2) << "\n";
  }

  auto cleanUpReceived(std::string received_path) const -> void override {
    ::remove(received_path.c_str());
  }
};

inline auto constexpr test_path{"packages/compiler/tests/"};

inline auto load_test_cases(std::string dir) {
  return std::filesystem::directory_iterator(std::string{test_path} + dir);
}

inline auto load_tokens(std::string const &filepath) {
  std::ifstream ifs{filepath};
  if (ifs) {
    auto data = json::parse(ifs);
    return data.at("tokens").get<std::vector<dao::token>>();
  }

  throw std::runtime_error{"failed to open data/" + filepath};
}

#endif

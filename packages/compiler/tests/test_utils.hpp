#pragma once
#ifdef CTEST

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

inline auto load_tokens(std::string const &fname) {
  std::ifstream ifs{"packages/compiler/tests/data/" + fname};
  if (ifs) {
    auto data = json::parse(ifs);
    return data.at("tokens").get<std::vector<dao::token>>();
  }

  throw std::runtime_error{"failed to open data/" + fname};
}

#endif

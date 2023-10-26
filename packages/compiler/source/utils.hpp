#pragma once

#include <fstream>

namespace dao {

  inline auto fread(char const *fpath) -> std::string {
    std::ifstream file{fpath};
    if (file.is_open()) {
      auto first = std::istreambuf_iterator<char>{file.rdbuf()};
      auto last  = std::istreambuf_iterator<char>{};
      return std::string{first, last};
    } else {
      throw std::runtime_error{"failed to open source file"};
    }
  }

} // namespace dao

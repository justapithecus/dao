#include <fstream>
#include <iostream>
#include <sstream>

#include "lexer.hpp"

namespace dao {

  auto fread(char const *fpath) -> std::string {
    std::ifstream file{fpath};
    if (file.is_open()) {
      auto begin = std::istreambuf_iterator<char>{file.rdbuf()};
      auto end   = std::istreambuf_iterator<char>{};
      return std::string{begin, end};
    } else {
      throw std::runtime_error{"failed to open source file"};
    }
  }

  auto lex(std::string_view fpath) -> std::vector<token> {
    std::vector<token> tokens{};

    std::cout << fpath << std::endl;
    auto source = dao::fread(fpath.data());
    std::cout << source << std::endl;

    auto          src_ptr = source.data();
    unsigned char state{lexical_state_final};

    do {
      unsigned char ch;
      auto          eqc = ch_to_eqc[(ch = *src_ptr++)];

      state = lex_trans[state + eqc];

      switch (state) {}

    } while (state > lexical_state_final);

    return tokens;
  }
} // namespace dao

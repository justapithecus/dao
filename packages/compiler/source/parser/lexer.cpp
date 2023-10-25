#include <fstream>
#include <sstream>

#include "lexer.hpp"

namespace dao {

  auto fread(char const *fpath) -> std::string {
    auto begin = std::istreambuf_iterator<char>{std::ifstream{fpath}.rdbuf()};
    auto end   = std::istreambuf_iterator<char>{};
    return std::string{begin, end};
  }

  auto lex(std::string_view fpath) -> std::vector<token> {
    std::vector<token> tokens{};

    auto          source  = dao::fread(fpath.data());
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

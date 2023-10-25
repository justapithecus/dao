#include <fstream>
#include <iostream>
#include <sstream>

#include "lexer.hpp"
#include "state_machine.h"

namespace dao {

  auto fread(char const *fpath) -> std::string {
    std::ifstream file{fpath};
    if (file.is_open()) {
      auto first = std::istreambuf_iterator<char>{file.rdbuf()};
      auto last  = std::istreambuf_iterator<char>{};
      return std::string{first, last};
    } else {
      throw std::runtime_error{"failed to open source file"};
    }
  }

  auto lex(std::string_view fpath) -> std::vector<dao::token> {
    std::vector<dao::token> tokens{};
    dao::lexeme             lexeme{};
    auto                    source{dao::fread(fpath.data())};
    auto                    src_ptr{source.data()};
    unsigned char           state{lexical_state_final};

    do {
      unsigned char ch;
      auto          eqc = ch_to_eqc[(ch = *src_ptr++)];

      state = lex_trans[state + eqc];

      switch (state) {
      [[likely]] case lexical_state_next_char:
        ++lexeme.col_num;
        break;
      case lexical_state_identifier:
        lexeme.token_kind = token_kind_identifier;
        break;
      case lexical_state_identifier_end:
        lexeme.update_repr(src_ptr, 1, 0);

        // end of lexeme requires re-evaluation
        // auto reeval = ch_reeval[ch]
        // src_ptr += reeval;
        // TODO(andrew): update column position according to lexeme length, if re-evaluation occurred
        // lexeme.col_num += reeval

        lexeme.reset();
        tokens.emplace_back(lexeme.as_token());
        break;
      }

    } while (state > lexical_state_final);

    return tokens;
  }
} // namespace dao

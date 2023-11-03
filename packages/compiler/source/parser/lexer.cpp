#include "lexer.hpp"

#include "../utils.hpp"
#include "state_machine.h"

namespace dao {

  auto lex(std::string_view fpath) -> std::vector<dao::token> {
    std::vector<dao::token> tokens{};
    dao::lexeme             lexeme{};
    auto                    source{dao::fread(fpath.data())};
    auto                    src_ptr{source.data()};
    unsigned char           state{lexical_state_next_char};

    do {
      unsigned char ch;
      auto          eqc = equivalence_class[(ch = *src_ptr++)];

      state = transition[state + eqc];
      src_ptr += ch_rewind[state];
      // lexeme.col_num += ch_rewind[state];
      lexeme.len += inside[state];

      switch (state) {
      case lexical_state_new_line:
        lexeme.repr = "\\n";
        tokens.emplace_back(lexeme.as_token(token_kind::e_new_line));
        [[fallthrough]];
      case lexical_state_next_char: {
        ++lexeme.col_num;
        break;
      }
      case lexical_state_separator: {
        lexeme.update_repr(src_ptr, 0);
        tokens.emplace_back(lexeme.as_token(token_kind::e_separator));
        break;
      }
      case lexical_state_operator: {
        lexeme.update_repr(src_ptr, 0);
        tokens.emplace_back(lexeme.as_token(token_kind::e_operator));
        break;
      }
      case lexical_state_numeral_end: {
        lexeme.update_repr(src_ptr, 0);
        tokens.emplace_back(lexeme.as_token(token_kind::e_numeral));
        break;
      }
      case lexical_state_identifier_end: {
        lexeme.update_repr(src_ptr, 0);
        if (auto it{keywords.find(lexeme.repr)}; it != keywords.end()) {
          tokens.emplace_back(lexeme.as_token(it->second));
        } else {
          tokens.emplace_back(lexeme.as_token(token_kind::e_identifier));
        }
        break;
      }
      case lexical_state_string_literal_end: {
        lexeme.update_repr(src_ptr, -1);
        tokens.emplace_back(lexeme.as_token(token_kind::e_literal));
        break;
      }
      default:
        break;
      }

    } while (state > lexical_state_final);

    return tokens;
  }
} // namespace dao

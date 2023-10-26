#include "../utils.hpp"

#include "lexer.hpp"
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
      case lexical_state_next_char: {
        ++lexeme.col_num;
        break;
      }
      case lexical_state_identifier: {
        lexeme.token_kind = token_kind_identifier;
        break;
      }
      case lexical_state_numeral: {
        lexeme.token_kind = token_kind_numeral;
        break;
      }
      case lexical_state_operator:
        lexeme.token_kind = token_kind_operator;
        [[fallthrough]];
      case lexical_state_identifier_end:
        [[fallthrough]];
      case lexical_state_numeral_end: {
        lexeme.update_repr(src_ptr, 0);
        lexeme.reset();
        tokens.emplace_back(lexeme.as_token());
        break;
      }
      default:
        break;
      }

    } while (state > lexical_state_final);

    return tokens;
  }
} // namespace dao

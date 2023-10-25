#pragma once

// Glyphs are categories comprising individual characters or strings of characters, e.g.
// '+' and '->', respectively.
typedef enum glyph {
  glyph_layout,
  glyph_white_space,
  glyph_new_line,
  glyph_letter,
  glyph_number,
  glyph_separator,
  glyph_operator,
  glyph_eof,
  glyph_count,
} glyph;

typedef enum lexical_state {
  lexical_state_final,
  lexical_state_next_char,
  lexical_state_separator,
  lexical_state_identifier,
  lexical_state_identifier_end,
  lexical_state_numeral,
  lexical_state_numeral_end,
  lexical_state_new_line,
  lexical_state_eof,
  lexical_state_error,
  lexical_state_count,
} lexical_state;

// Character to character-equivalence classes lookup table
//
// character-equivalence classes are pre-computed offsets used to index into
// the lexical state machine lookup table, to determine the next lexical state.
extern unsigned char const ch_to_eqc[256];

typedef enum token_kind {
  token_kind_identifier,
  token_kind_numeral,
  token_kind_separator,
  token_kind_operator,
  token_kind_count,
} token_kind;

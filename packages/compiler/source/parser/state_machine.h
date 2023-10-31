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
  lexical_state_operator,
  lexical_state_new_line,
  lexical_state_eof,
  lexical_state_error,
  lexical_state_count,
} lexical_state;

// Character to character-equivalence classes lookup table
//
// Character-equivalence classes are pre-computed offsets used to index into
// the lexical state transitions lookup table.
extern unsigned char const equivalence_class[256];

// Lexical state transitions lookup table
//
// Index using the current state offset by a a character-equivalence class, to lookup
// the next lexical state.
#define LEX_TRANS_SIZE ((glyph_count) * (lexical_state_count))
#pragma clang diagnostic   push
#pragma clang diagnostic   ignored "-Wdeprecated-enum-enum-conversion"
extern unsigned char const transition[LEX_TRANS_SIZE];
#pragma clang diagnostic   pop

// Character rewind
//
// Certain lexical states, typically those terminating a lexeme, requires shifting back 1
// character, to start lexing anew.
extern char const ch_rewind[lexical_state_count];

// True if inside an identifier, value, or operator
extern unsigned char const inside[lexical_state_count];

extern unsigned char const binary_op_precedence[256];

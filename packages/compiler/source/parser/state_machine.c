#include "state_machine.h"

#define offset(kind) ((kind) * (lexical_state_count))

unsigned char const equivalence_class[256] = {
  // Layout
  [0]          = offset(glyph_eof),
  [1 ... 9]    = offset(glyph_layout),
  ['\n']       = offset(glyph_new_line),
  [11 ... 12]  = offset(glyph_layout),
  ['\r']       = offset(glyph_new_line),
  [14 ... 31]  = offset(glyph_layout),
  [' ']        = offset(glyph_white_space),
  // String Literals
  ['"']        = offset(glyph_double_quote),
  // Special
  ['!']        = offset(glyph_operator),
  ['(']        = offset(glyph_separator),
  [')']        = offset(glyph_separator),
  ['*']        = offset(glyph_operator),
  ['+']        = offset(glyph_operator),
  ['-']        = offset(glyph_operator),
  // TODO(andrew): glyph_accessor to distinguish for identifiers and numerals
  ['.']        = offset(glyph_operator),
  ['/']        = offset(glyph_operator),
  ['<']        = offset(glyph_operator),
  ['=']        = offset(glyph_operator),
  ['>']        = offset(glyph_operator),
  // Numbers
  [48 ... 57]  = offset(glyph_number),
  // Letters
  [65 ... 90]  = offset(glyph_letter),
  ['_']        = offset(glyph_letter),
  [97 ... 122] = offset(glyph_letter),
};

// initiating states to start processing a new lexeme after completing a previous one
// clang-format off
#define initiating_states(state) \
  [state + offset(glyph_layout)]       = lexical_state_next_char, \
  [state + offset(glyph_white_space)]  = lexical_state_next_char, \
  [state + offset(glyph_new_line)]     = lexical_state_new_line, \
  [state + offset(glyph_letter)]       = lexical_state_identifier, \
  [state + offset(glyph_number)]       = lexical_state_numeral, \
  [state + offset(glyph_separator)]    = lexical_state_separator, \
  [state + offset(glyph_operator)]     = lexical_state_operator, \
  [state + offset(glyph_double_quote)] = lexical_state_string_literal

// reduce applies a state transition
#define reduce(prev_state, transition, next_state) \
  [prev_state + offset(transition)] = next_state

unsigned char const transition[LEX_TRANS_SIZE] = {
  initiating_states(lexical_state_next_char),
  initiating_states(lexical_state_new_line),
  initiating_states(lexical_state_identifier_end),
  initiating_states(lexical_state_numeral_end),
  initiating_states(lexical_state_separator),
  initiating_states(lexical_state_operator),
  initiating_states(lexical_state_string_literal_end),

  reduce(lexical_state_identifier, glyph_white_space, lexical_state_identifier_end),
  reduce(lexical_state_identifier, glyph_new_line, lexical_state_identifier_end),
  reduce(lexical_state_identifier, glyph_letter, lexical_state_identifier),
  reduce(lexical_state_identifier, glyph_number, lexical_state_identifier),
  reduce(lexical_state_identifier, glyph_operator, lexical_state_identifier_end),
  reduce(lexical_state_identifier, glyph_separator, lexical_state_identifier_end),
  reduce(lexical_state_identifier, glyph_double_quote, lexical_state_identifier_end),

  reduce(lexical_state_numeral, glyph_white_space, lexical_state_numeral_end),
  reduce(lexical_state_numeral, glyph_new_line, lexical_state_numeral_end),
  reduce(lexical_state_numeral, glyph_letter, lexical_state_numeral_end),
  reduce(lexical_state_numeral, glyph_number, lexical_state_numeral),
  reduce(lexical_state_numeral, glyph_operator, lexical_state_numeral_end),
  reduce(lexical_state_numeral, glyph_separator, lexical_state_numeral_end),
  reduce(lexical_state_numeral, glyph_double_quote, lexical_state_numeral_end),

  reduce(lexical_state_string_literal, glyph_white_space, lexical_state_string_literal),
  reduce(lexical_state_string_literal, glyph_new_line, lexical_state_error),
  reduce(lexical_state_string_literal, glyph_letter, lexical_state_string_literal),
  reduce(lexical_state_string_literal, glyph_number, lexical_state_string_literal),
  reduce(lexical_state_string_literal, glyph_operator, lexical_state_string_literal),
  reduce(lexical_state_string_literal, glyph_separator, lexical_state_string_literal),
  reduce(lexical_state_string_literal, glyph_double_quote, lexical_state_string_literal_end),
};
// clang-format on

char const ch_rewind[lexical_state_count] = {
  [lexical_state_identifier_end] = -1,
  [lexical_state_numeral_end]    = -1,
};

unsigned char const inside[lexical_state_count] = {
  [lexical_state_separator]      = 1,
  [lexical_state_operator]       = 1,
  [lexical_state_identifier]     = 1,
  [lexical_state_numeral]        = 1,
  [lexical_state_string_literal] = 1,
};

unsigned char const binary_op_precedence[256] = {
  ['>'] = 10, // lowest
  ['<'] = 10,
  ['+'] = 20,
  ['-'] = 20,
  ['*'] = 40,
  ['/'] = 40, // highest
};

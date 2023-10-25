#include "state_machine.h"

#define offset(kind) ((kind) * (lexical_state_count))

unsigned char const ch_to_eqc[256] = {
  // Layout
  [0]          = offset(glyph_eof),
  [1 ... 9]    = offset(glyph_layout),
  ['\n']       = offset(glyph_new_line),
  ['\r']       = offset(glyph_new_line),
  [11 ... 12]  = offset(glyph_layout),
  [14 ... 31]  = offset(glyph_layout),
  [' ']        = offset(glyph_white_space),
  // Special
  ['!']        = offset(glyph_operator),
  ['(']        = offset(glyph_operator),
  [')']        = offset(glyph_operator),
  ['*']        = offset(glyph_operator),
  ['+']        = offset(glyph_operator),
  ['-']        = offset(glyph_operator),
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
  [state + offset(glyph_layout)]      = lexical_state_next_char, \
  [state + offset(glyph_white_space)] = lexical_state_next_char, \
  [state + offset(glyph_new_line)]    = lexical_state_new_line, \
  [state + offset(glyph_letter)]      = lexical_state_identifier, \
  [state + offset(glyph_number)]      = lexical_state_numeral, \
  [state + offset(glyph_separator)]   = lexical_state_separator, \
  [state + offset(glyph_operator)]    = lexical_state_operator

// reduce applies a state transition
#define reduce(prev_state, transition, next_state) \
  [previous_state + offset(transition)] = next_state
// clang-format on

unsigned char const lex_trans[LEX_TRANS_SIZE] = {
  initiating_states(lexical_state_next_char),
  initiating_states(lexical_state_new_line),
  initiating_states(lexical_state_identifier_end),
  initiating_states(lexical_state_numeral_end),
  initiating_states(lexical_state_separator),
  initiating_states(lexical_state_operator),
};

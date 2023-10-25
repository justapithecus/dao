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

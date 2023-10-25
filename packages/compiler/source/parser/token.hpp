#pragma once

#include <string_view>

#include "state_machine.h"

struct token {
  token_kind       kind;
  std::string_view repr;
  std::size_t      len;
};

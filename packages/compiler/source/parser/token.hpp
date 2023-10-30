#pragma once

#include <string>

#include "state_machine.h"

namespace dao {

  struct token {
    std::string  repr;
    std::uint8_t kind;
  };

  static_assert(sizeof(dao::token) <= 32);

} // namespace dao

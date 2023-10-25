#pragma once

#include <string>

#include "state_machine.h"

namespace dao {

  struct token {
    std::string  repr;
    std::uint8_t kind;
    std::uint8_t len;
  };

  static_assert(sizeof(dao::token) <= 32);

#ifdef CTEST
  auto operator<<(std::ostream &os, token const &tok) -> std::ostream &;
#endif

} // namespace dao

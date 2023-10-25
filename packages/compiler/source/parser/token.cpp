#include <iomanip>

#include "token.hpp"

namespace dao {

  auto operator<<(std::ostream &os, token const &tok) -> std::ostream & {
    os << std::setw(12);

    switch (tok.kind) {
    default:
      os << "Unknown";
      break;
    }

    return os << " | " << tok.repr << " |";
  }
} // namespace dao

#include <iomanip>

#include "state_machine.h"
#include "token.hpp"

#define token_desc(kind, desc)                                                 \
  case (kind):                                                                 \
    os << (desc);                                                              \
    break

namespace dao {

  auto operator<<(std::ostream &os, token const &tok) -> std::ostream & {
    os << std::setw(12);

    switch (tok.kind) {
      token_desc(token_kind_identifier, "Identifier");
      token_desc(token_kind_numeral, "Numeral");
      token_desc(token_kind_operator, "Operator");
      token_desc(token_kind_separator, "Separator");
    default:
      os << "Unknown";
      break;
    }

    return os << " | " << tok.repr << " |";
  }
} // namespace dao

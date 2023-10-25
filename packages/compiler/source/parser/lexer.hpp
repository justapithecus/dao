#include <string_view>
#include <vector>

#include "token.hpp"

namespace dao {
  auto lex(std::string_view fpath) -> std::vector<token>;
}

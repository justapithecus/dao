#include "token.hpp"

namespace dao {
  ankerl::unordered_dense::map<std::string, dao::token_kind> const keywords{
    {"external", token_kind::e_keyword_external},
    {"function", token_kind::e_keyword_function},
    {"if", token_kind::e_keyword_if},
    {"then", token_kind::e_keyword_then},
    {"else", token_kind::e_keyword_else},
    {"alias", token_kind::e_keyword_alias},
    {"as", token_kind::e_keyword_as},
    {"ptr", token_kind::e_keyword_as},
    {"mutable", token_kind::e_keyword_as},
    {"readonly", token_kind::e_keyword_as},
  };
}

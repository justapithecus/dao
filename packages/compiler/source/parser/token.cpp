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
    {"ptr", token_kind::e_keyword_ptr},
    {"mutable", token_kind::e_keyword_mutable},
    {"readonly", token_kind::e_keyword_readonly},
  };

  ankerl::unordered_dense::map<token_kind, type_qualifier> const
    token_to_type_qualifier{
      {token_kind::e_keyword_mutable, type_qualifier::e_mutable},
      {token_kind::e_keyword_readonly, type_qualifier::e_readonly},
    };
} // namespace dao

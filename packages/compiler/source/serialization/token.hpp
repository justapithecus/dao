#pragma once
#ifdef CTEST

#include <ankerl/unordered_dense.h>
#include <iomanip>

#include "../parser/token.hpp"
#include "json.hpp"

#define token_desc(kind, desc)                                                 \
  case (kind):                                                                 \
    os << (desc);                                                              \
    break

namespace dao {

  inline auto operator<<(std::ostream &os, dao::token const &tok)
    -> std::ostream & {
    os << std::setw(12);

    switch (tok.kind) {
      token_desc(token_kind::e_new_line, "New Line");
      token_desc(token_kind::e_identifier, "Identifier");
      token_desc(token_kind::e_numeral, "Numeral");
      token_desc(token_kind::e_operator, "Operator");
      token_desc(token_kind::e_separator, "Separator");
      token_desc(token_kind::e_literal, "String Literal");
      token_desc(token_kind::e_keyword_external, "Keyword");
      token_desc(token_kind::e_keyword_function, "Keyword");
      token_desc(token_kind::e_keyword_if, "Keyword");
      token_desc(token_kind::e_keyword_then, "Keyword");
      token_desc(token_kind::e_keyword_else, "Keyword");
      os << "Unknown";
      break;
    }

    return os << " | " << tok.repr << " |";
  }

  inline ankerl::unordered_dense::map<std::string, dao::token_kind>
    str_to_kind = {
      {"new_line", token_kind::e_new_line},
      {"identifier", token_kind::e_identifier},
      {"numeral", token_kind::e_numeral},
      {"operator", token_kind::e_operator},
      {"separator", token_kind::e_separator},
      {"literal", token_kind::e_literal},
  };

  inline auto to_json(json &j, dao::token const &tok) {
    j = json{{"kind", tok.kind}, {"repr", tok.repr}};
  }

  inline auto from_json(json const &j, dao::token &tok) {
    std::string kind{};
    j.at("kind").get_to(kind);
    j.at("repr").get_to(tok.repr);

    if (auto it{keywords.find(tok.repr)}; it != keywords.end()) {
      tok.kind = it->second;
    } else {
      tok.kind = str_to_kind.at(kind);
    }
  }

} // namespace dao

#endif

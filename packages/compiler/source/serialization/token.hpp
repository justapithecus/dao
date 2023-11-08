#pragma once
#ifdef CTEST

#include <ankerl/unordered_dense.h>

#include "../parser/token.hpp"

#define token_desc(kind, desc)                                                 \
  case (kind):                                                                 \
    os << (desc);                                                              \
    break

namespace dao {

  inline ankerl::unordered_dense::map<dao::token_kind, std::string> kind_to_str{
    {token_kind::e_end_of_file, "end_of_file"},
    {token_kind::e_new_line, "new_line"},
    {token_kind::e_identifier, "identifier"},
    {token_kind::e_numeral, "numeral"},
    {token_kind::e_operator, "operator"},
    {token_kind::e_separator, "separator"},
    {token_kind::e_string_literal, "literal"},
  };

  inline ankerl::unordered_dense::map<std::string, dao::token_kind> str_to_kind{
    {"end_of_file", token_kind::e_end_of_file},
    {"new_line", token_kind::e_new_line},
    {"identifier", token_kind::e_identifier},
    {"numeral", token_kind::e_numeral},
    {"operator", token_kind::e_operator},
    {"separator", token_kind::e_separator},
    {"literal", token_kind::e_string_literal},
  };

  inline auto to_json(json &j, dao::token const &tok) {
    std::string kind{};

    if (auto it{kind_to_str.find(tok.kind)}; it != kind_to_str.end()) {
      kind = it->second;
    } else {
      kind = "keyword";
    }

    j = json{
      {"kind", kind},
      {"repr", tok.repr},
    };
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

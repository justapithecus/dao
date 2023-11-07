#include "types.hpp"

namespace dao {

  ankerl::unordered_dense::map<std::string, builtin_type_kind> const
    builtin_types{
      {"int8", builtin_type_kind::e_int8},
      {"int32", builtin_type_kind::e_int32},
    };
}

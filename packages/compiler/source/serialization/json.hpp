#pragma once
#ifdef CTEST

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlohmann {
  template <typename T>
  struct adl_serializer<std::unique_ptr<T>> {
    static auto to_json(json &j, std::unique_ptr<T> const &ptr) -> void {
      if (ptr.get()) {
        j = *ptr;
      } else {
        j = nullptr;
      }
    }
  };
} // namespace nlohmann

#endif

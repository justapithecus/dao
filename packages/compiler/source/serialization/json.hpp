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

  template <typename T>
  struct adl_serializer<std::optional<T>> {
    static auto to_json(json &j, std::optional<T> const &opt) -> void {
      if (opt.has_value()) {
        j = opt.value();
      } else {
        j = nullptr;
      }
    }

    static auto from_json(json const &j, std::optional<T> &opt) -> void {
      if (j.is_null()) {
        opt = std::nullopt;
      } else {
        opt = j.get<T>();
      }
    }
  };
} // namespace nlohmann

#endif
